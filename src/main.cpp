#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <random>
#include <chrono>

// Classe TicTacToe
class TicTacToe {
private:
    std::array<std::array<char, 3>, 3> tabuleiro;       // Tabuleiro do jogo
    std::mutex acesso_tabuleiro;        // Mutex para controle de acesso ao tabuleiro
    std::condition_variable vez_cv;     // Variável de condição para alternância de turnos
    char jogador_atual;     // Jogador atual ('X' ou 'O')
    bool jogo_finalizado;       // Estado do jogo
    char vencedor;      // Vencedor do jogo

public:
    TicTacToe() {
        // Inicializar o tabuleiro e as variáveis do jogo

        for (auto& linha : tabuleiro) {
            linha.fill(' ');
        }

        jogador_atual = 'X';
        jogo_finalizado = false;
        vencedor = ' ';
    }

    void display_board() {
        // Exibir o tabuleiro no console

        std::cout << "\n";

        for (int i = 0; i < 3; ++i) {
            std::cout << " ";

            for (int j = 0; j < 3; ++j) {
                std::cout << tabuleiro[i][j];
                if (j < 2) std::cout << " | ";
            }
            std::cout << "\n";
            if (i < 2) std::cout << "---+---+---\n";
        }
        std::cout << "\n";
    }

    bool make_move(char jogador, int linha, int coluna) {
         // Implementar a lógica para realizar uma jogada no tabuleiro
        // Utilizar mutex para controle de acesso
        // Utilizar variável de condição para alternância de turnos

        std::unique_lock<std::mutex> lock(acesso_tabuleiro);
        vez_cv.wait(lock, [&] { return jogador_atual == jogador && !jogo_finalizado; });

        if (tabuleiro[linha][coluna] != ' ') {
            return false;
        }

        tabuleiro[linha][coluna] = jogador;
        display_board();

        if (check_win(jogador)) {
            jogo_finalizado = true;
            vencedor = jogador;
        } else if (check_draw()) {
            jogo_finalizado = true;
            vencedor = 'D';
        } else {
            jogador_atual = (jogador == 'X') ? 'O' : 'X';
        }

        vez_cv.notify_all();

        return true;
    }

    bool check_win(char jogador) {
        // Verificar se o jogador atual venceu o jogo

        for (int i = 0; i < 3; ++i) {
            if ((tabuleiro[i][0] == jogador && tabuleiro[i][1] == jogador && tabuleiro[i][2] == jogador) ||
                (tabuleiro[0][i] == jogador && tabuleiro[1][i] == jogador && tabuleiro[2][i] == jogador))
                return true;
        }

        return (tabuleiro[0][0] == jogador && tabuleiro[1][1] == jogador && tabuleiro[2][2] == jogador) ||
               (tabuleiro[0][2] == jogador && tabuleiro[1][1] == jogador && tabuleiro[2][0] == jogador);
    }

    bool check_draw() {
        // Verificar se houve um empate

        for (const auto& linha : tabuleiro) {

            for (char celula : linha) {

                if (celula == ' ') return false;
            }
        }
        return true;
    }

    bool is_game_over() {
        // Retornar se o jogo terminou
        std::lock_guard<std::mutex> lock(acesso_tabuleiro);

        return jogo_finalizado;
    }

    char get_winner() {
        // Retornar o vencedor do jogo ('X', 'O', ou 'D' para empate)
        std::lock_guard<std::mutex> lock(acesso_tabuleiro);

        return vencedor;
    }
};

// Classe Player
class Player {
private:
    TicTacToe& jogo;     // Referência para o jogo
    char simbolo;       // Símbolo do jogador ('X' ou 'O')
    std::string estrategia;     // Estratégia do jogador
    std::mt19937 gerador;   

public:
    Player(TicTacToe& jogo_ref, char s, std::string estrat) 
        : jogo(jogo_ref), simbolo(s), estrategia(estrat), gerador(std::random_device{}()) {}

    void play() {
        // Executar jogadas de acordo com a estratégia escolhida

        if (estrategia == "sequencial") {
            play_sequential();
        } else if (estrategia == "aleatorio") {
            play_random();
        }
    }

private:
    void play_sequential() {
        //Implementar a estratégia sequencial de jogadas
        for (int i = 0; !jogo.is_game_over(); ++i) {

            for (int linha = 0; linha < 3; ++linha) {

                for (int coluna = 0; coluna < 3; ++coluna) {

                    if (jogo.make_move(simbolo, linha, coluna)) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }

                    if (jogo.is_game_over()) return;
                }
            }
        }
    }

    void play_random() {
        // Implementar a estratégia aleatória de jogadas
        std::uniform_int_distribution<> dist(0, 2);

        while (!jogo.is_game_over()) {
            int linha, coluna;
            do {
                linha = dist(gerador);
                coluna = dist(gerador);
            } while (!jogo.make_move(simbolo, linha, coluna) && !jogo.is_game_over());

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};

// Função principal
int main() {

    // Inicializar o jogo e os jogadores

    // Criar as threads para os jogadores

    // Aguardar o término das threads

    // Exibir o resultado final do jogo

    TicTacToe jogo;

    Player jogador1(jogo, 'X', "sequencial");
    Player jogador2(jogo, 'O', "aleatorio");

    std::thread t1(&Player::play, &jogador1);
    std::thread t2(&Player::play, &jogador2);

    t1.join();
    t2.join();

    char vencedor = jogo.get_winner();

    if (vencedor == 'D') {
        std::cout << "Empate!\n";
    } else {
        std::cout << "Jogador " << vencedor << " venceu!\n";
    }

    return 0;
}
