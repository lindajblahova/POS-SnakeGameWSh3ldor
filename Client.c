#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>


/// Rozmery plochy
#define N  50
#define M  18

int field1[M][N] = {0}; /// Pozicia pre hraca 1
int field2[M][N] = {0}; /// Pozicia pre hraca 2
int direction = 4; /// Smer pohybu (1-4 clockwise)
int head1 = 5;
int tail1 = 1;
int y_1 = (M / 4) * 3 + 1;
int x1 = N - 10;
int current_score1 = 0;

int head2 = 5;
int tail2 = 1;
int y2 = M / 4;
int x2 = 10;
int current_score2 = 0;

int fruit_generated = 1;
int fruit_x = 10;
int fruit_y = 7;
int fruit_value = 0;

int play = 3;
int game_status = 0;

int sockt, n;
int sockfd;
struct sockaddr_in serv_addr;
struct hostent* server;

char buffer[256];

int ** server_field;
int ** client_field;

void draw_arena();
int key_hit();
void connect_to_server(char *argv[]);
void play_game();
void * listen_server(void* arg);
void draw_game();
void snake_init();
void draw_arena();
void draw_game_over();
void countdown();
void eat_fruit();
void check_collision();
void step(int change);
void start_screen();
void loser_screen();
void winner_screen();
void opponent_left_screen();
void you_left_screen();
void get_info();
void send_info();
void set_values(char args[256]);
int send_message(char *message);

int main(int argc, char *argv[]) {


    server_field = malloc(M * sizeof(int *));
    for (int i = 0; i < M; ++i)
        server_field[i] = malloc(N * sizeof(int));

    client_field = malloc(M * sizeof(int *));
    for (int i = 0; i < M; ++i)
        client_field[i] = malloc(N * sizeof(int));

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 3;
    }

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to socket");
        return 4;
    }

    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    /// color pairs used for color graphics
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);

    srand(time(NULL));


    start_screen();
    bzero(buffer,256);
    strcpy(buffer,"play");
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("Error writing to socket");
        return 5;
    }



    int c = 0;
    int ch = 0;
    int direction_change = 4;

    //snake_init();
    draw_arena();

    /// Countdown from 3
    //countdown();


    //get_info();
    mvprintw(M + 3, 0, "Game stat je: %d", play);
    refresh();
    while(play == 3) {


        /// Get input
        if (key_hit()) {
            c = getch();
            if (c == 97)
                direction_change = 4;
            if (c == 100)
                direction_change = 2;
            if (c == 120)
                direction_change = 0;
            if (c == 119)
                direction_change = 1;
            if (c == 115)
                direction_change = 3;
        }
        usleep(100000);
        n = write(sockfd, &direction_change, sizeof(direction_change));
        if (n < 0)
        {
            perror("Error reading from socket");
            return 6;
        }
        /*mvprintw(M + 3, 0, "Odoslal som dir");
        refresh();

        usleep(100);*/
        n = read(sockfd, &field2, sizeof(field2));
        if (n < 0)
        {
            perror("Error reading from socket");
            return 6;
        }
        /*mvprintw(M + 4, 0, "Prijal som pole 2");
        refresh();

        usleep(100);*/
        n = read(sockfd, &field1, sizeof(field1));
        if (n < 0) {
            perror("Error writing to socket");
            return 6;
        }
        /*mvprintw(M + 5, 0, "Prijal som pole 1");
        refresh();

        usleep(100);*/
        n = read(sockfd, &head2, sizeof(head2));
        if (n < 0) {
            perror("Error writing to socket");
            return 6;
        }
        /*mvprintw(M + 6, 0, "Prijal som hlavu 2");
        refresh();

        usleep(100);*/
        n = read(sockfd, &head1, sizeof(head1));
        if (n < 0) {
            perror("Error writing to socket");
            return 6;
        }
        /*mvprintw(M + 7, 0, "Prijal som hlavu 1");
        refresh();

        usleep(100);*/
        n = read(sockfd, &current_score2, sizeof(current_score2));
        if (n < 0) {
            perror("Error writing to socket");
            return 6;
        }
        /*mvprintw(M + 8, 0, "Prijal som score 2");
        refresh();

        usleep(100);*/
        n = read(sockfd, &current_score1, sizeof(current_score1));
        if (n < 0) {
            perror("Error writing to socket");
            return 6;
        }
        /*mvprintw(M + 9, 0, "Prijal som score 1");
        refresh();

        usleep(100);*/
        n = read(sockfd, &fruit_x, sizeof(fruit_x));
        if (n < 0) {
            perror("Error writing to socket");
            return 6;
        }
        /*mvprintw(M + 10, 0, "Prijal som fruit x");
        refresh();

        usleep(100);*/
        n = read(sockfd, &fruit_y, sizeof(fruit_y));
        if (n < 0) {
            perror("Error writing to socket");
            return 6;
        }
        /*mvprintw(M + 11, 0, "Prijal som fruit y");
        refresh();

        usleep(100);*/
        n = read(sockfd, &fruit_value, sizeof(fruit_value));
        if (n < 0) {
            perror("Error writing to socket");
            return 6;
        }
        /*mvprintw(M + 12, 0, "Prijal som fruit value");
        refresh();

        usleep(100);*/
        n = read(sockfd, &fruit_generated, sizeof(fruit_generated));
        if (n < 0) {
            perror("Error writing to socket");
            return 6;
        }
        /*mvprintw(M + 13, 0, "Prijal som is fruit");
        refresh();

        usleep(100);*/
        n = read(sockfd, &play, sizeof(play));
        if (n < 0) {
            perror("Error writing to socket");
            return 6;
        }
        /*mvprintw(M + 14, 0, "Prijal som game stat");
        refresh();*/

        /*for (int i = 3; i < 15; ++i) {
            mvprintw(M+i, 0, "                                 ");
        }
        refresh();*/


        /*for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                field1[i][j] = client_field[i][j];
                field2[i][j] = server_field[i][j];
            }

        }*/


        /// 1. Nastav udaje - zavolaj GET na server
        //get_info();
        /// 2. Posli udaje o sebe na server
        //send_info();

        /// Print play_game area
        draw_game();



        //usleep(200000);
    }
    refresh();

    draw_game_over();

    sleep(2);
    switch (play) {
        case 2:
            winner_screen();
            break;
        case 1:
            loser_screen();
            break;
        default:
            you_left_screen();
            break;
    }



    close(sockfd);
    endwin();
    for (int i = 0; i < N; ++i)
        free(client_field[i]);
    free(client_field);

    for (int i = 0; i < N; ++i)
        free(server_field[i]);
    free(server_field);
    return 0;
}


int key_hit() {
    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}

void connect_to_server(char *argv[]) {

    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        exit(2);
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockt = socket(AF_INET, SOCK_STREAM, 0);
    if (sockt < 0) {
        perror("Error creating socket");
        exit(3);
    }

    if(connect(sockt, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to socket");
        exit(4);
    }

    usleep(100);
}

void play_game() {

    int c = 0;
    int direction_change = direction;

    snake_init();
    draw_arena();

    /// Countdown from 3
    //countdown();


    //get_info();

    while(play) {

        /// 1. Nastav udaje - zavolaj GET na server
        //get_info();
        /// 2. Posli udaje o sebe na server
        //send_info();

        /// Print play_game area
        draw_game();

        /// Get input
        if (key_hit()) {
            c = getch();
            if (c == 97)
                direction_change = 4;
            if (c == 100)
                direction_change = 2;
            if (c == 120)
                play = 0;
            if (c == 119)
                direction_change = 1;
            if (c == 115)
                direction_change = 3;
        }

        step(direction_change);

        eat_fruit();
        check_collision();
        usleep(300000);
    }
    refresh();

    draw_game_over();

    sleep(2);

    if (current_score1 == 0)
        loser_screen();
    if (current_score1 > 0)
        winner_screen();
}

/**
 * Initialization of snake
 */
void snake_init() {
    int j1 = x1;
    int j2 = x2;
    for (int i = 1; i <= head1; ++i) {
        field1[y_1][j1++] = head1 - (i - 1);
        field2[y2][++j2 - head2] = i;
    }
}

void draw_game() {
    for(int i = 1; i <= M - 1; i++){
        for (int j = 1; j <= N - 1; j++) {
            if (((field2[i][j] > 0) && (field2[i][j] < head2)) || ((field1[i][j] > 0) && (field1[i][j] < head1))) {
                mvprintw(i, j, "o");
            } else if ((field2[i][j] == head2) || (field1[i][j] == head1)) {
                mvprintw(i, j, "x");
            } else if (fruit_generated == 1 && j == fruit_x && i == fruit_y) {
                mvprintw(i, j, "%d",fruit_value);
            } else {
                mvprintw(i, j, " ");
            }
            /// !!! Bacha na ELSE vetvu !!!
        }
    }
    mvprintw(M + 2, (N/2) - 17, "Your Score: %d  Opponent's Score: %d", current_score1, current_score2);
    move(M + 3, 0);

    move(M + 10, 0);
    /*for(int i = 1; i <= M - 1; i++){
        for (int j = 1; j <= N - 1; j++) {
            printw("%d", field2[i][j]);
        }
        printw("\n");
    }
    printw("\n");*/
    refresh();
}

void draw_arena() {
    move(0,0);
    attr_on(COLOR_PAIR(2),0);
    for(int i=0;i<=M;i++){
        for (int j = 0; j <= N; ++j) {
            if ((i == 0 && j == 0) || (i == 0 && j == N) || (i == M && j == 0) || (i == M && j == N)) {
                printw("+");
            }
            if ((i == 0 && j > 0 && j < N) || (i == M && j > 0 && j < N)) {
                printw("-");
            }
            if ((i > 0 && i < M && j == 0) || (i > 0 && i < M && j == N)) {
                printw("|");
            }
            if (i > 0 && i < M && j > 0 && j < N) {
                printw(" ");
            }
        }
        printw("\n");
    }
    attr_off(COLOR_PAIR(2),0);
    refresh();
}

void draw_game_over() {
    attr_on(COLOR_PAIR(3),0);
    mvprintw((M / 2) - 1, (N/2) - 6, "           ");
    mvprintw(M / 2, (N/2) - 6, " Game OVER ");
    mvprintw((M / 2) + 1, (N/2) - 6, "           ");
    move(0,0);
    attr_off(COLOR_PAIR(3),0);
    move(M + 1, 0);
    refresh();
}

void countdown() {
    draw_game();
    for (int i = 3; i > 0; --i) {
        attr_on(COLOR_PAIR(3),0);
        mvprintw(M / 2, (N/2), "%d", i);
        move(M + 1,0);
        attr_off(COLOR_PAIR(3),0);
        refresh();
        sleep(1);
    }
}

/**
 * Checks if snake is at fruit position
 */
void eat_fruit(){
    if ((fruit_x) == x1 && fruit_y == y_1) {
        fruit_generated = 0;
        for (int i = 0; i < fruit_value; ++i) {
            tail1--;
            current_score1++;
        }
    }
}

/**
 * Checks snake collision with itself
 */
void check_collision() {
    if (field1[y_1][x1] != 0)
        play = 0;
}

/**
 * Change direction of movement
 * @param change
 */
void step(int change) {
    switch (change) {
        case 1:
            if (direction == 3)
                break;
            direction = 1;
            break;
        case 2:
            if (direction == 4)
                break;
            direction = 2;
            break;
        case 3:
            if (direction == 1)
                break;
            direction = 3;
            break;
        case 4:
            if (direction == 2)
                break;
            direction = 4;
            break;
        default:
            break;
    }

    switch (direction) {
        case 1:
            if (y_1-- <= 1)
                y_1 = M - 1;
            break;
        case 2:
            if (x1++ >= N - 1)
                x1 = 1;
            break;
        case 3:
            if (y_1++ >= M - 1)
                y_1 = 1;
            break;
        case 4:
            if (x1-- <= 1)
                x1 = N - 1;
            break;
        default:
            break;
    }

    field1[y_1][x1] = ++head1;
    field2[y2][x2] = ++head2;

    /// shift tail1
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            if (field1[i][j] == tail1)
                field1[i][j] = 0;
            if (field2[i][j] == tail2)
                field2[i][j] = 0;
        }
    }
    tail1++;
}

void start_screen() {
    system("clear");
    draw_arena();
    attr_on(COLOR_PAIR(1),0);
    mvprintw(M/2 - 4, N/2 - 20, "    ________         ________  ");
    mvprintw(M/2 - 3,  N/2 - 20, "   /        \\       /        \\        0 ");
    mvprintw(M/2 - 2,  N/2 - 20 , "  /  /----\\  \\     /  /----\\  \\       0  ");
    mvprintw(M/2 - 1,  N/2 - 20 , "  |  |    |  |     |  |    |  |      / \\ ");
    mvprintw(M/2 ,  N/2 - 20, "  |  |    |  |     |  |    |  |     /  |  ");
    mvprintw(M/2 + 1,  N/2 - 20," (o  o)   \\   \\___/   /    \\   \\___/   / ");
    mvprintw(M/2 + 2,  N/2 - 20 , "  \\__/     \\         /      \\         /  ");
    mvprintw(M/2 + 3,  N/2 - 20 , "            --------         -------- 	");
    attr_off(COLOR_PAIR(1),0);

    mvprintw(M/2 + 5,  N/2 - 12 ,"Start when you are ready!");
    refresh();

    attr_on(COLOR_PAIR(4),0);
    while (getch() != '\n')  /// caka na enter
    {
        mvprintw(M/2 + 6,  N/2 - 10 , "Press ENTER to START.");
        move(M + 1, 0);
        refresh();
        sleep(1);
        mvprintw(M/2 + 6,  N/2 - 10 ,"                     ");
        move(M + 1, 0);
        refresh();
        sleep(1);
    }

    attr_off(COLOR_PAIR(4),0);

}

void loser_screen() {
    //system("clear");
    draw_arena();
    attr_on(COLOR_PAIR(3),0);
    mvprintw(M/2 - 7, N/2 - 11,"         ________   ");
    mvprintw(M/2 - 6,  N/2 - 11,"        /        \\  ");
    mvprintw(M/2 - 5,  N/2 - 11,"       /  /----\\  \\  ");
    mvprintw(M/2 - 4,  N/2 - 11,"       |  |    |  |  ");
    mvprintw(M/2 - 3,  N/2 - 11,"       |  |    |  | ");
    mvprintw(M/2 - 2,  N/2 - 11,"      (o  o)   |  | ");
    mvprintw(M/2 - 1,  N/2 - 11,"  |\\   \\__/    |  | ");
    mvprintw(M/2 ,  N/2 - 11,"  | \\__________/  / ");
    mvprintw(M/2 + 1,  N/2 - 11,"  \\              / ");
    mvprintw(M/2 + 2,  N/2 - 11,"   -------------  	");
    attr_off(COLOR_PAIR(3),0);

    mvprintw(M/2 + 4,  N/2 - 11,"	   Game OVER!");
    mvprintw(M/2 + 5,  N/2 - 11, "	 Your SCORE: %d !", current_score1);

    mvprintw(M + 2, (N/2) - 16, "                                    ");
    refresh();

    attr_on(COLOR_PAIR(1),0);
    while (getch() != '\n'){
        mvprintw(M/2 + 6, N/2 - 13,"  Press ENTER to FINISH !");
        move(M + 1, 0);
    }
    attr_off(COLOR_PAIR(1),0);
}

void winner_screen() {
    draw_arena();
    attr_on(COLOR_PAIR(4),0);
    mvprintw(M/2 - 7, N/2 - 22,"      _________________________________  ");
    mvprintw(M/2 - 6, N/2 - 22,"     /                                 \\ ");
    mvprintw(M/2 - 5, N/2 - 22,"    /  /-----------------------------\\  \\ ");
    mvprintw(M/2 - 4, N/2 - 22,"    |  |          . . . . . . .      |  | ");
    mvprintw(M/2 - 3, N/2 - 22,"    |  |          |\\/\\/\\/\\/\\/\\|      |  | ");
    mvprintw(M/2 - 2, N/2 - 22,"   (o  o)         | o o o o o |      |  | ");
    mvprintw(M/2 - 1, N/2 - 22,"    \\__/    |\\    |___________|      |  | 	");
    mvprintw(M/2 , N/2 - 22,"            | \\______________________/  /  ");
    mvprintw(M/2 + 1, N/2 - 22,"            \\                          /  ");
    mvprintw(M/2 + 2, N/2 - 22,"             --------------------------  ");
    attr_off(COLOR_PAIR(4),0);

    mvprintw(M/2 + 4, N/2 - 10,"     You WON!");
    mvprintw(M/2 + 5, N/2 - 10, "  Your SCORE: %d !", current_score1);

    mvprintw(M + 2, (N/2) - 16, "                                    ");
    refresh();

    attr_on(COLOR_PAIR(1),0);
    while (getch() != '\n') {
        mvprintw(M/2 + 6, N/2 - 13,"  Press ENTER to FINISH !");
        move(M + 1, 0);
    }
    attr_off(COLOR_PAIR(1),0);

}

void opponent_left_screen() {
    //system("clear");
    draw_arena();
    attr_on(COLOR_PAIR(3),0);
    mvprintw(M/2 - 7, N/2 - 10,"         / \\   ");
    mvprintw(M/2 - 6,  N/2 - 10,"        /   \\  ");
    mvprintw(M/2 - 5,  N/2 - 10,"       /     \\");
    mvprintw(M/2 - 4,  N/2 - 10,"      /   _   \\ ");
    mvprintw(M/2 - 3,  N/2 - 10,"     /   | |   \\ ");
    mvprintw(M/2 - 2,  N/2 - 10,"    /    | |    \\ ");
    mvprintw(M/2 - 1,  N/2 - 10,"   /     |_|     \\ ");
    mvprintw(M/2 ,  N/2 - 10,   "  /               \\ ");
    mvprintw(M/2 + 1,  N/2 - 10," /        O        \\ ");
    mvprintw(M/2 + 2,  N/2 - 10,"/___________________\\ ");
    attr_off(COLOR_PAIR(3),0);

    mvprintw(M/2 + 4,  N/2 - 20,"Ooops! Looks like your opponent LEFT !");
    mvprintw(M/2 + 5,  N/2 - 5, "We 're SORRY!");

    mvprintw(M + 2, (N/2) - 16, "                                    ");
    refresh();

    attr_on(COLOR_PAIR(1),0);
    while (getch() != '\n'){
        mvprintw(M/2 + 6, N/2 - 13,"  Press ENTER to FINISH !");
        move(M + 1, 0);
    }
    attr_off(COLOR_PAIR(1),0);
}

void you_left_screen() {
    //system("clear");
    draw_arena();
    attr_on(COLOR_PAIR(3),0);
    mvprintw(M/2 - 7, N/2 - 10,"    /-----------\\     ");
    mvprintw(M/2 - 6,  N/2 - 10,"   /             \\  ");
    mvprintw(M/2 - 5,  N/2 - 10,"  /               \\  ");
    mvprintw(M/2 - 4,  N/2 - 10," |     O     O     |  ");
    mvprintw(M/2 - 3,  N/2 - 10," |                 | ");
    mvprintw(M/2 - 2,  N/2 - 10," |     _______     |");
    mvprintw(M/2 - 1,  N/2 - 10,"  \\   /       \\   / ");
    mvprintw(M/2 ,  N/2 - 10,   "   \\             / ");
    mvprintw(M/2 + 1,  N/2 - 10,"    \\___________/");
    attr_off(COLOR_PAIR(3),0);

    mvprintw(M/2 + 4,  N/2 - 9,"You LEFT the GAME!");
    mvprintw(M/2 + 5,  N/2 - 12, "We HOPE you'll come BACK!");

    mvprintw(M + 2, (N/2) - 16, "                                    ");
    refresh();

    attr_on(COLOR_PAIR(1),0);
    while (getch() != '\n'){
        mvprintw(M/2 + 6, N/2 - 13,"  Press ENTER to FINISH !");
        move(M + 1, 0);
    }
    attr_off(COLOR_PAIR(1),0);
}

void get_info() {

    bzero(buffer,256);
    strcpy(buffer, "getinfo");
    n = write(sockt, buffer, strlen(buffer));
    bzero(buffer, strlen(buffer));
    mvprintw(M + 8, 0, "Som tu");
    //usleep(1000);

    n = read(sockt, buffer, strlen(buffer));
    mvprintw(M + 9, 0, "Odpoved: %d   ", n);

    /*if (n > 0) {
        //TODO: set info
        mvprintw(M + 10, 0, "Odpoved servera: %s   ", buffer);
        break;
    }*/
    mvprintw(M + 10, 0, "Odpoved servera: %s   ", buffer);
    mvprintw(M + 11, 0, "Koniec GET requestu   ");
}

void send_info() {
    char info[256];
    bzero(info, strlen(info));
    sprintf(info, "%d;%d;%d;%d;%d;%d;%d;%d", x1, y_1, head1, tail1, current_score1, game_status);
    send_message(info);
}

void set_values(char args[256]) {
    int i = 5;
    /*char * token = strtok(args, ";");
        while( token != NULL ) {
            switch (i) {
                case 0:
                    x2 = atoi(token);
                    break;
                case 1:
                    y2 = atoi(token);
                    break;
                case 2:
                    head2 = atoi(token);
                    break;
                case 3:
                    tail2 = atoi(token);
                    break;
                case 4:
                    current_score2 = atoi(token);
                    break;
                case 5:
                    fruit_x = atoi(token);
                    break;
                case 6:
                    fruit_y = atoi(token);
                    break;
                case 7:
                    game_status = atoi(token);
                    break;
                default:
                    mvprintw(M + 1, 0, "Something goes wrong!");
            }
            i++;
            token = strtok(NULL, ";");
        }*/
    mvprintw(M + 5, 0, args);

    /*char string[50];
    bzero(string, 50);
    strcpy(string, args);
    // Extract the first token
    char * token = strtok(string, ";");
    // loop through the string to extract all other tokens
    while( token != NULL ) {
        mvprintw(M + i, 0, " %s", token ); //printing each token
        i++;
        token = strtok(NULL, ";"); }
        */

}

int send_message(char *message) {
    bzero(buffer,256);
    strcpy(buffer, message);
    n = write(sockt, buffer, strlen(buffer));
    if (n < 0)
        return -1;
    bzero(buffer,256);
    return 0;
}