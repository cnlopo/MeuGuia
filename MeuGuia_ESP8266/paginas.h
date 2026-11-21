#ifndef paginas
#define paginas

const char mainPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <link rel="stylesheet" href="style.css">
    </head>
    <body>
    <style>
    %STYLE%
    </style>
        <header class="header">
            <a href="/" class="logo">MEU GUIA</a>
            <div class="wificonnection">%WIFIPLACEHOLDER%</div>
            <div class="nav">
                <a href="/endereco" class="navlinks">
                    Endereço
            </a>
            <a href="/config" class="navlinks">
                Configurar
            </a>
            </div>
        </header>
        <div class="container">
            <a href="/endereco">
                <button class="option">Definir Endereço</button>
            </a>
            <br>
            <a href="/config">
                <button class="option">Configurar WiFi</button>
            </a>
        </div>
    </body>
</html>
)rawliteral";
const char configPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <link rel="stylesheet" href="style.css">
    </head>
    <body>
    <style>
    %STYLE%
    </style>
        <header class="header">
            <a href="/" class="logo">MEU GUIA</a>
            <div class="wificonnection">%WIFIPLACEHOLDER%</div>
            <div class="nav">
                <a href="/endereco" class="navlinks">
                    Endereço
            </a>
            <a href="/config" class="navlinks">
                Configurar
            </a>
            </div>
        </header>
        <div class="container">
            <form action="/wifi">
                <label for="ssid" class="formlabel">Nome da rede</label>
                <input type="text" name="ssid" id="ssid" class="textbox">
                <br>
                <label for="pass" class="formlabel">Senha da rede</label>
                <input type="text" name="pass" id="pass" class="textbox">
                <br>
                <input type="submit" value="Enviar" class="submitbtn">
            </form>
        </div>
    </body>
</html>
)rawliteral";

const char enderecoPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <link rel="stylesheet" href="style.css">
    </head>
    <body>
    <style>
    %STYLE%
    </style>
        <header class="header">
            <a href="/" class="logo">MEU GUIA</a>
            <div class="wificonnection">%WIFIPLACEHOLDER%</div>
            <div class="nav">
                <a href="/endereco" class="navlinks">
                    Endereço
            </a>
            <a href="/config" class="navlinks">
                Configurar
            </a>
            </div>
        </header>
        <div class="container">
            <form action="/query">
                <label for="rua" class="formlabel">Digite o nome da rua</label>
                <input type="text" name="rua" id="rua" class="textbox">
                <br>
                <input type="submit" value="Enviar" class="submitbtn">
            </form>
            <div class="endSalvo">
            <H1>Destino atualmente salvo</H1>
                %ENDERECOSALVO%
            </div>
        </div>
    </body>
</html>
)rawliteral";
const char style[] PROGMEM = R"rawliteral(
body{
        margin: 0;
}
.header{
        background-color: #347abb;
        text-align: right;
        display: flex;
        justify-content: space-between;
        align-items: center;
}
.wificonnection{
        color: #FFFFFF;
        font-size: 30px;
        padding: 20px;
        text-decoration: none;
}
.navlinks{
        color: #FFFFFF;
        font-size: 30px;
        padding: 20px;
        text-decoration: none;
}
.logo{
        color: #FFFFFF;
        font-size: 50px;
        padding: 20px;
        text-decoration: none;
}
.container{
        display:flexbox;
        height: auto;
        width: 50%;
        margin-left: 25%;
        margin-top: 10%;
        text-align: center;
        padding-top: 20px;
        padding-bottom: 20px; 
}
.option{
        margin: 10px;
        border-radius: 15px;
        padding: 10px;
        font-size: 50px;
        color:#FFFFFF;
        background-color: #347abb;
}
.formlabel{
        font-size: 50px;
}
.textbox{
        font-size: 50px;
        padding: 10px;
}
.submitbtn{
        background-color: #347abb;
        font-size: 40px;
        padding: 30px;
        margin-top: 20px;
}
)rawliteral";
#endif