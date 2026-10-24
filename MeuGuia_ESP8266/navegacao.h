#ifndef NAVEGACAO_H
#define NAVEGACAO_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEITURAS_JANELA_PASSOU 6

typedef enum {
    NIVEL_LONGE,
    NIVEL_PREPARAR,
    NIVEL_DAR_SINAL,
    NIVEL_CHEGANDO,
    NIVEL_PASSOU,
    NIVEL_ENCERRADA
} NivelProximidade;

typedef struct {
    double destino_latitude;
    double destino_longitude;

    bool tem_posicao_valida;
    double ultima_latitude;
    double ultima_longitude;
    uint32_t tempo_ultima_ms;

    /* Leituras que pularam demais; viram a posição válida após LEITURAS_CONFIRMAR_SALTO
       seguidas concordando. */
    int saltos_seguidos;
    double salto_latitude;
    double salto_longitude;
    uint32_t tempo_salto_ms;

    NivelProximidade nivel;
    double menor_distancia_metros;
    bool checagem_passou_ativa;

    double distancias_recentes[LEITURAS_JANELA_PASSOU];
    uint32_t tempos_recentes_ms[LEITURAS_JANELA_PASSOU];
    int proxima_leitura;
    int leituras_guardadas;
} EstadoNavegacao;

double distancia_metros(double latitude_origem, double longitude_origem, double latitude_destino,
                        double longitude_destino);

void iniciar_navegacao(EstadoNavegacao *estado, double destino_latitude, double destino_longitude);

/* tempo_ms vem de millis() no .ino; nos testes, de valores inventados.
   Devolve o nível atual a cada leitura, mesmo descartada: quem chama deve reagir à mudança
   de nível. PASSOU toca uma vez; ENCERRADA é fim de viagem, sem vibração. */
NivelProximidade atualizar_navegacao(EstadoNavegacao *estado, double latitude, double longitude,
                                     uint32_t tempo_ms);

#ifdef __cplusplus
}
#endif

#endif
