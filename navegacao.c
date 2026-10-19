#include "navegacao.h"

#include <math.h>

#define PI_RAD 3.14159265358979323846
#define RAIO_TERRA_METROS 6371000.0

#define LIMITE_PREPARAR_METROS 400.0
#define LIMITE_DAR_SINAL_METROS 200.0
#define LIMITE_CHEGANDO_METROS 50.0
#define MARGEM_HISTERESE_METROS 10.0

#define VELOCIDADE_MAXIMA_M_S 30.0
#define LEITURAS_CONFIRMAR_SALTO 3

#define MARGEM_PASSOU_METROS 50.0
#define JANELA_MINIMA_SEGUNDOS 3.0
#define LIMITE_VELOCIDADE_ONIBUS_M_S 3.0

static double graus_para_radianos(double graus) {
    return graus * PI_RAD / 180.0;
}

static double seno_ao_quadrado_da_metade(double angulo_rad) {
    double seno = sin(angulo_rad / 2.0);
    return seno * seno;
}

double distancia_metros(double latitude_origem, double longitude_origem, double latitude_destino,
                        double longitude_destino) {
    double variacao_latitude = graus_para_radianos(latitude_destino - latitude_origem);
    double variacao_longitude = graus_para_radianos(longitude_destino - longitude_origem);
    double haversine = seno_ao_quadrado_da_metade(variacao_latitude) +
                       cos(graus_para_radianos(latitude_origem)) *
                           cos(graus_para_radianos(latitude_destino)) *
                           seno_ao_quadrado_da_metade(variacao_longitude);
    return 2.0 * RAIO_TERRA_METROS * asin(sqrt(haversine));
}

/* A subtração em uint32_t continua certa quando o millis() volta a zero. */
static double segundos_entre(uint32_t antes_ms, uint32_t agora_ms) {
    return (double)(agora_ms - antes_ms) / 1000.0;
}

static bool dentro_da_velocidade_maxima(double latitude_antes, double longitude_antes,
                                        uint32_t antes_ms, double latitude_agora,
                                        double longitude_agora, uint32_t agora_ms) {
    double salto_metros =
        distancia_metros(latitude_antes, longitude_antes, latitude_agora, longitude_agora);
    return salto_metros <= VELOCIDADE_MAXIMA_M_S * segundos_entre(antes_ms, agora_ms);
}

static void guardar_posicao_valida(EstadoNavegacao *estado, double latitude, double longitude,
                                   uint32_t tempo_ms) {
    estado->tem_posicao_valida = true;
    estado->ultima_latitude = latitude;
    estado->ultima_longitude = longitude;
    estado->tempo_ultima_ms = tempo_ms;
    estado->saltos_seguidos = 0;
}

static void registrar_salto(EstadoNavegacao *estado, double latitude, double longitude,
                            uint32_t tempo_ms) {
    bool concorda_com_salto_anterior =
        estado->saltos_seguidos > 0 &&
        dentro_da_velocidade_maxima(estado->salto_latitude, estado->salto_longitude,
                                    estado->tempo_salto_ms, latitude, longitude, tempo_ms);
    estado->saltos_seguidos = concorda_com_salto_anterior ? estado->saltos_seguidos + 1 : 1;
    estado->salto_latitude = latitude;
    estado->salto_longitude = longitude;
    estado->tempo_salto_ms = tempo_ms;
}

static void esvaziar_janela(EstadoNavegacao *estado) {
    estado->proxima_leitura = 0;
    estado->leituras_guardadas = 0;
}

static bool aceitar_leitura(EstadoNavegacao *estado, double latitude, double longitude,
                            uint32_t tempo_ms) {
    if (!estado->tem_posicao_valida ||
        dentro_da_velocidade_maxima(estado->ultima_latitude, estado->ultima_longitude,
                                    estado->tempo_ultima_ms, latitude, longitude, tempo_ms)) {
        guardar_posicao_valida(estado, latitude, longitude, tempo_ms);
        return true;
    }
    registrar_salto(estado, latitude, longitude, tempo_ms);
    if (estado->saltos_seguidos < LEITURAS_CONFIRMAR_SALTO) {
        return false;
    }
    /* A diferença de distância do salto não é movimento do ônibus e não pode virar ritmo. */
    esvaziar_janela(estado);
    guardar_posicao_valida(estado, latitude, longitude, tempo_ms);
    return true;
}

static void guardar_na_janela(EstadoNavegacao *estado, double distancia, uint32_t tempo_ms) {
    estado->distancias_recentes[estado->proxima_leitura] = distancia;
    estado->tempos_recentes_ms[estado->proxima_leitura] = tempo_ms;
    estado->proxima_leitura = (estado->proxima_leitura + 1) % LEITURAS_JANELA_PASSOU;
    if (estado->leituras_guardadas < LEITURAS_JANELA_PASSOU) {
        estado->leituras_guardadas++;
    }
}

/* Distância igual ao limite já conta como dentro do nível. */
static NivelProximidade nivel_pela_distancia(double distancia) {
    if (distancia <= LIMITE_CHEGANDO_METROS) {
        return NIVEL_CHEGANDO;
    }
    if (distancia <= LIMITE_DAR_SINAL_METROS) {
        return NIVEL_DAR_SINAL;
    }
    if (distancia <= LIMITE_PREPARAR_METROS) {
        return NIVEL_PREPARAR;
    }
    return NIVEL_LONGE;
}

/* De DAR_SINAL em diante o nível não volta atrás, então só PREPARAR precisa de margem para sair. */
static NivelProximidade nivel_com_histerese(NivelProximidade atual, double distancia) {
    NivelProximidade pela_distancia = nivel_pela_distancia(distancia);
    if (pela_distancia >= atual) {
        return pela_distancia;
    }
    if (atual == NIVEL_PREPARAR && distancia > LIMITE_PREPARAR_METROS + MARGEM_HISTERESE_METROS) {
        return NIVEL_LONGE;
    }
    return atual;
}

static NivelProximidade nivel_pelo_ritmo(const EstadoNavegacao *estado) {
    int mais_nova = (estado->proxima_leitura + LEITURAS_JANELA_PASSOU - 1) % LEITURAS_JANELA_PASSOU;
    int mais_antiga =
        estado->leituras_guardadas < LEITURAS_JANELA_PASSOU ? 0 : estado->proxima_leitura;
    double segundos = segundos_entre(estado->tempos_recentes_ms[mais_antiga],
                                     estado->tempos_recentes_ms[mais_nova]);
    if (segundos < JANELA_MINIMA_SEGUNDOS) {
        return estado->nivel;
    }
    double ritmo_m_s =
        (estado->distancias_recentes[mais_nova] - estado->distancias_recentes[mais_antiga]) /
        segundos;
    /* Ritmo exatamente no limite conta como pessoa a pé. */
    return ritmo_m_s > LIMITE_VELOCIDADE_ONIBUS_M_S ? NIVEL_PASSOU : NIVEL_ENCERRADA;
}

/* Se o ônibus sai de novo da zona do primeiro aviso, a viagem para este destino acabou. */
static NivelProximidade nivel_depois_de_passar(const EstadoNavegacao *estado, double distancia) {
    if (distancia > LIMITE_PREPARAR_METROS) {
        return NIVEL_ENCERRADA;
    }
    if (distancia < estado->menor_distancia_metros) {
        return nivel_pela_distancia(distancia);
    }
    return NIVEL_PASSOU;
}

static NivelProximidade proximo_nivel(const EstadoNavegacao *estado, double distancia) {
    if (estado->nivel == NIVEL_PASSOU) {
        return nivel_depois_de_passar(estado, distancia);
    }
    if (estado->checagem_passou_ativa &&
        distancia > estado->menor_distancia_metros + MARGEM_PASSOU_METROS) {
        return nivel_pelo_ritmo(estado);
    }
    return nivel_com_histerese(estado->nivel, distancia);
}

void iniciar_navegacao(EstadoNavegacao *estado, double destino_latitude, double destino_longitude) {
    *estado = (EstadoNavegacao){
        .destino_latitude = destino_latitude,
        .destino_longitude = destino_longitude,
        .nivel = NIVEL_LONGE,
        .menor_distancia_metros = INFINITY,
    };
}

NivelProximidade atualizar_navegacao(EstadoNavegacao *estado, double latitude, double longitude,
                                     uint32_t tempo_ms) {
    if (estado->nivel == NIVEL_ENCERRADA ||
        !aceitar_leitura(estado, latitude, longitude, tempo_ms)) {
        return estado->nivel;
    }
    double distancia =
        distancia_metros(latitude, longitude, estado->destino_latitude, estado->destino_longitude);
    guardar_na_janela(estado, distancia, tempo_ms);
    estado->nivel = proximo_nivel(estado, distancia);
    if (distancia < estado->menor_distancia_metros) {
        estado->menor_distancia_metros = distancia;
    }
    if (estado->nivel == NIVEL_DAR_SINAL || estado->nivel == NIVEL_CHEGANDO) {
        estado->checagem_passou_ativa = true;
    }
    return estado->nivel;
}
