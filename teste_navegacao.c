#include <math.h>
#include <stdio.h>

#include "navegacao.h"

/* Constantes próprias do teste: se o módulo errar um dígito, o teste não erra junto. */
#define PI_TESTE 3.14159265358979323846
#define RAIO_TERRA_TESTE_METROS 6371000.0
#define METROS_POR_GRAU (2.0 * PI_TESTE * RAIO_TERRA_TESTE_METROS / 360.0)

#define TOLERANCIA_A_MAO_METROS 0.01
#define MILESIMO_DE_GRAU 0.001

#define LATITUDE_RIBEIRAO -21.2
#define LONGITUDE_RIBEIRAO -47.81
#define LATITUDE_OUTRO_PONTO -21.17
#define LONGITUDE_OUTRO_PONTO -47.84

#define INTERVALO_GPS_MS 1000u
#define ONIBUS_METROS_POR_LEITURA 10.0
#define PESSOA_METROS_POR_LEITURA 1.4

static int falhas = 0;

static void confere(const char *nome, double esperado, double calculado, double tolerancia) {
    if (fabs(esperado - calculado) <= tolerancia) {
        printf("ok    %s\n", nome);
        return;
    }
    printf("FALHA %s: esperado %.4f, calculado %.4f\n", nome, esperado, calculado);
    falhas++;
}

static void testa_distancia(void) {
    double metros_em_milesimo_de_grau = METROS_POR_GRAU * MILESIMO_DE_GRAU;
    double cosseno_ribeirao = cos(LATITUDE_RIBEIRAO * PI_TESTE / 180.0);

    confere("mesmo ponto", 0.0,
            distancia_metros(LATITUDE_RIBEIRAO, LONGITUDE_RIBEIRAO, LATITUDE_RIBEIRAO,
                             LONGITUDE_RIBEIRAO),
            TOLERANCIA_A_MAO_METROS);
    confere("0,001 grau de latitude", metros_em_milesimo_de_grau,
            distancia_metros(LATITUDE_RIBEIRAO, LONGITUDE_RIBEIRAO,
                             LATITUDE_RIBEIRAO + MILESIMO_DE_GRAU, LONGITUDE_RIBEIRAO),
            TOLERANCIA_A_MAO_METROS);
    confere("0,001 grau de longitude no equador", metros_em_milesimo_de_grau,
            distancia_metros(0.0, 0.0, 0.0, MILESIMO_DE_GRAU), TOLERANCIA_A_MAO_METROS);
    confere("0,001 grau de longitude em Ribeirao", metros_em_milesimo_de_grau * cosseno_ribeirao,
            distancia_metros(LATITUDE_RIBEIRAO, LONGITUDE_RIBEIRAO, LATITUDE_RIBEIRAO,
                             LONGITUDE_RIBEIRAO + MILESIMO_DE_GRAU),
            TOLERANCIA_A_MAO_METROS);
    confere("simetria",
            distancia_metros(LATITUDE_RIBEIRAO, LONGITUDE_RIBEIRAO, LATITUDE_OUTRO_PONTO,
                             LONGITUDE_OUTRO_PONTO),
            distancia_metros(LATITUDE_OUTRO_PONTO, LONGITUDE_OUTRO_PONTO, LATITUDE_RIBEIRAO,
                             LONGITUDE_RIBEIRAO),
            TOLERANCIA_A_MAO_METROS);
}

static void confere_nivel(const char *nome, NivelProximidade esperado, NivelProximidade obtido) {
    confere(nome, esperado, obtido, 0.0);
}

/* Nos cenários o destino é o ponto de Ribeirão e o ônibus anda pelo meridiano,
   então a distância é exatamente os metros ao norte (ou ao sul, se negativos). */
static EstadoNavegacao estado;
static uint32_t agora_ms;

static void comecar_viagem(uint32_t tempo_inicial_ms) {
    iniciar_navegacao(&estado, LATITUDE_RIBEIRAO, LONGITUDE_RIBEIRAO);
    agora_ms = tempo_inicial_ms;
}

static NivelProximidade ler_no_mesmo_instante(double metros_ao_norte) {
    return atualizar_navegacao(&estado, LATITUDE_RIBEIRAO + metros_ao_norte / METROS_POR_GRAU,
                               LONGITUDE_RIBEIRAO, agora_ms);
}

static NivelProximidade ler_a_metros(double metros_ao_norte) {
    agora_ms += INTERVALO_GPS_MS;
    return ler_no_mesmo_instante(metros_ao_norte);
}

static NivelProximidade andar(double de_metros, double ate_metros, double metros_por_leitura) {
    int leituras = (int)(fabs(ate_metros - de_metros) / metros_por_leitura + 0.5);
    double passo = ate_metros > de_metros ? metros_por_leitura : -metros_por_leitura;
    NivelProximidade nivel = estado.nivel;
    for (int i = 1; i <= leituras; i++) {
        nivel = ler_a_metros(de_metros + passo * i);
    }
    return nivel;
}

static void chegar_perto_do_ponto(void) {
    ler_a_metros(305);
    andar(305, 5, ONIBUS_METROS_POR_LEITURA);
}

static void testa_onibus_se_aproximando(void) {
    comecar_viagem(0);
    confere_nivel("aproximando: 600 m e LONGE", NIVEL_LONGE, ler_a_metros(600));
    confere_nivel("aproximando: 450 m e LONGE", NIVEL_LONGE,
                  andar(600, 450, ONIBUS_METROS_POR_LEITURA));
    confere_nivel("aproximando: 350 m e PREPARAR", NIVEL_PREPARAR,
                  andar(450, 350, ONIBUS_METROS_POR_LEITURA));
    confere_nivel("aproximando: 150 m e DAR_SINAL", NIVEL_DAR_SINAL,
                  andar(350, 150, ONIBUS_METROS_POR_LEITURA));
    confere_nivel("aproximando: 20 m e CHEGANDO", NIVEL_CHEGANDO,
                  andar(150, 20, ONIBUS_METROS_POR_LEITURA));
}

static void testa_histerese_em_400_metros(void) {
    comecar_viagem(0);
    ler_a_metros(420);
    confere_nivel("histerese 400: entra em 395", NIVEL_PREPARAR, ler_a_metros(395));
    confere_nivel("histerese 400: fica em 405", NIVEL_PREPARAR, ler_a_metros(405));
    confere_nivel("histerese 400: sai em 415", NIVEL_LONGE, ler_a_metros(415));
}

static void testa_oscilacao_em_50_metros(void) {
    double oscilacao_metros[] = {53, 47, 55, 49, 58, 46};
    int saidas_de_chegando = 0;

    comecar_viagem(0);
    ler_a_metros(80);
    andar(80, 52, 4);
    confere_nivel("oscilacao 50: entra em 48", NIVEL_CHEGANDO, ler_a_metros(48));
    for (int i = 0; i < (int)(sizeof oscilacao_metros / sizeof oscilacao_metros[0]); i++) {
        if (ler_a_metros(oscilacao_metros[i]) != NIVEL_CHEGANDO) {
            saidas_de_chegando++;
        }
    }
    confere("oscilacao 50: nao sai de CHEGANDO", 0, saidas_de_chegando, 0.0);
}

static void testa_onibus_passando_reto(void) {
    comecar_viagem(0);
    chegar_perto_do_ponto();
    confere_nivel("passou reto: 45 m depois ainda CHEGANDO", NIVEL_CHEGANDO,
                  andar(5, -45, ONIBUS_METROS_POR_LEITURA));
    confere_nivel("passou reto: 75 m depois e PASSOU", NIVEL_PASSOU,
                  andar(-45, -75, ONIBUS_METROS_POR_LEITURA));
    confere_nivel("passou reto: voltando continua PASSOU", NIVEL_PASSOU,
                  andar(-75, -15, ONIBUS_METROS_POR_LEITURA));
    confere_nivel("passou reto: abaixo da menor distancia volta a CHEGANDO", NIVEL_CHEGANDO,
                  ler_a_metros(-3));
}

static void testa_pessoa_descendo_a_pe(void) {
    comecar_viagem(0);
    chegar_perto_do_ponto();
    for (int segundo = 0; segundo < 10; segundo++) {
        ler_a_metros(5);
    }
    confere_nivel("a pe: afastando a 1,4 m/s e ENCERRADA", NIVEL_ENCERRADA,
                  andar(5, 70, PESSOA_METROS_POR_LEITURA));
    confere_nivel("a pe: ENCERRADA e final", NIVEL_ENCERRADA,
                  andar(70, 3, ONIBUS_METROS_POR_LEITURA));
}

static void testa_salto_de_200_metros(void) {
    comecar_viagem(0);
    ler_a_metros(310);
    ler_a_metros(300);
    confere_nivel("salto: leitura isolada 200 m adiante e descartada", NIVEL_PREPARAR,
                  ler_a_metros(100));
    confere_nivel("salto: leitura normal seguinte e aceita", NIVEL_PREPARAR, ler_a_metros(285));
    ler_a_metros(100);
    confere_nivel("salto: segunda leitura concordando ainda descartada", NIVEL_PREPARAR,
                  ler_a_metros(95));
    confere_nivel("salto: terceira leitura concordando vira a posicao", NIVEL_DAR_SINAL,
                  ler_a_metros(90));
}

static void testa_onibus_parado_no_semaforo(void) {
    comecar_viagem(0);
    ler_a_metros(300);
    andar(300, 150, ONIBUS_METROS_POR_LEITURA);
    NivelProximidade nivel = estado.nivel;
    for (int segundo = 0; segundo < 90; segundo++) {
        nivel = ler_a_metros(segundo % 2 == 0 ? 146 : 154);
    }
    confere_nivel("semaforo: 90 s parado com ruido continua DAR_SINAL", NIVEL_DAR_SINAL, nivel);
}

static void testa_volta_do_millis(void) {
    comecar_viagem(0xFFFFFFFFu - 32000u);
    chegar_perto_do_ponto();
    confere_nivel("millis: CHEGANDO antes da volta a zero", NIVEL_CHEGANDO, estado.nivel);
    confere_nivel("millis: PASSOU com a janela atravessando a volta", NIVEL_PASSOU,
                  andar(5, -75, ONIBUS_METROS_POR_LEITURA));
}

static void testa_passou_e_seguiu_ate_500_metros(void) {
    comecar_viagem(0);
    chegar_perto_do_ponto();
    confere_nivel("passou e seguiu: 500 m depois e ENCERRADA", NIVEL_ENCERRADA,
                  andar(5, -505, ONIBUS_METROS_POR_LEITURA));
}

static void testa_passou_e_voltou_antes_de_400_metros(void) {
    comecar_viagem(0);
    chegar_perto_do_ponto();
    andar(5, -355, ONIBUS_METROS_POR_LEITURA);
    confere_nivel("passou e voltou antes de 400 m: continua PASSOU", NIVEL_PASSOU,
                  andar(-355, -105, ONIBUS_METROS_POR_LEITURA));
}

static void testa_leituras_no_mesmo_instante(void) {
    comecar_viagem(0);
    chegar_perto_do_ponto();
    confere_nivel("mesmo instante: posicao repetida mantem CHEGANDO", NIVEL_CHEGANDO,
                  ler_no_mesmo_instante(5));
    confere_nivel("mesmo instante: posicao diferente mantem CHEGANDO", NIVEL_CHEGANDO,
                  ler_no_mesmo_instante(60));
}

static void testa_salto_confirmado_afastando(void) {
    comecar_viagem(0);
    chegar_perto_do_ponto();
    ler_a_metros(205);
    ler_a_metros(205);
    confere_nivel("salto confirmado afastando: o salto nao vira ritmo", NIVEL_CHEGANDO,
                  ler_a_metros(205));
}

int main(void) {
    testa_distancia();
    testa_onibus_se_aproximando();
    testa_histerese_em_400_metros();
    testa_oscilacao_em_50_metros();
    testa_onibus_passando_reto();
    testa_pessoa_descendo_a_pe();
    testa_salto_de_200_metros();
    testa_onibus_parado_no_semaforo();
    testa_volta_do_millis();
    testa_passou_e_seguiu_ate_500_metros();
    testa_passou_e_voltou_antes_de_400_metros();
    testa_leituras_no_mesmo_instante();
    testa_salto_confirmado_afastando();

    if (falhas > 0) {
        printf("\n%d teste(s) falharam\n", falhas);
        return 1;
    }
    printf("\ntodos os testes passaram\n");
    return 0;
}
