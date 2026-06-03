/*
 * ============================================================
 *  VIEW — Interface visual aprimorada
 * ============================================================
 *  Melhorias:
 *  - Paleta de 256 cores (ANSI extended)
 *  - Borda decorativa ao redor do mapa
 *  - Painel lateral com info de estruturas em tempo real
 *  - Menu reformulado com opcao auto-play
 *  - HUD mais limpo com separadores visuais
 *  - Tela de scores com exibicao da BST
 *  - Animacoes mais suaves
 * ============================================================
 */

#include "pacman.h"

/* ======================== PLATAFORMA ======================== */

#ifndef _WIN32
static struct termios orig_termios;
#endif

/* Sprites opcionais em emoji (podem variar largura por terminal) */
static int g_use_emoji = 0;

void sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void enable_ansi(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);
    SetConsoleOutputCP(65001);
#endif
}

int kbhit_custom(void) {
#ifdef _WIN32
    return _kbhit();
#else
    int ch = getchar();
    if (ch != EOF) { ungetc(ch, stdin); return 1; }
    return 0;
#endif
}

int getch_custom(void) {
#ifdef _WIN32
    return _getch();
#else
    return getchar();
#endif
}

int manhattan(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

/* ======================== TERMINAL ======================== */

static void move_cursor(int x, int y) { printf("\033[%d;%dH", y + 1, x + 1); }
static void hide_cursor(void)  { printf("\033[?25l"); }
static void show_cursor(void)  { printf("\033[?25h"); }

void view_clear(void) { printf("\033[2J\033[H"); }
void view_flush(void) { fflush(stdout); }

void view_init(void) {
    enable_ansi();

    /*
     * Reduz flicker no console (especialmente no Windows Terminal):
     * acumula os vários printf do frame em buffer e dá flush 1x/frame.
     */
    setvbuf(stdout, NULL, _IOFBF, 1 << 20);

    /* Ativa modo emoji via variável de ambiente: PACMAN_EMOJI=1 */
    {
        const char *e = getenv("PACMAN_EMOJI");
        if (e && e[0] == '1') g_use_emoji = 1;
    }
#ifndef _WIN32
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
#endif
    hide_cursor();
    /* Fundo preto total */
    printf("\033[40m");
}

void view_cleanup(void) {
    show_cursor();
    printf(CLR_RESET "\033[49m");
#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
#endif
}

/* ======================== PRIMITIVAS ======================== */

static void print_abs(int col, int row, const char *color, const char *str) {
    move_cursor(col, row);
    printf("%s%s%s", color, str, CLR_RESET);
}

/* ======================== BORDA DO MAPA ======================== */

static void view_draw_map_border(void) {
    /* Calcula posicoes da borda ao redor do mapa */
    int left   = OFFSET_X - 1;
    int top    = OFFSET_Y - 1;
    int right  = OFFSET_X + MAP_W * CELL_W;
    int bottom = OFFSET_Y + MAP_H;

    /* Linha superior com titulo */
    move_cursor(left, top);
    printf("%s╔", CLR_WALL);
    for (int i = 0; i < MAP_W * CELL_W; i++) printf("═");
    printf("╗%s", CLR_RESET);

    /* Linhas laterais */
    for (int row = top + 1; row < bottom; row++) {
        move_cursor(left, row);
        printf("%s║%s", CLR_WALL, CLR_RESET);
        move_cursor(right, row);
        printf("%s║%s", CLR_WALL, CLR_RESET);
    }

    /* Linha inferior */
    move_cursor(left, bottom);
    printf("%s╚", CLR_WALL);
    for (int i = 0; i < MAP_W * CELL_W; i++) printf("═");
    printf("╝%s", CLR_RESET);
}

/* ======================== HEADER TOP ======================== */

static void view_draw_header(GameModel *m) {
    /* Linha 1: titulo e scores */
    move_cursor(OFFSET_X, 1);
    printf("%s 1UP %s", CLR_DIM, CLR_RESET);
    move_cursor(OFFSET_X + 4, 1);
    printf("%s%8d%s", CLR_SCORE, m->score, CLR_RESET);

    /* Titulo centralizado */
    int title_col = OFFSET_X + MAP_W * CELL_W / 2 - 5;
    move_cursor(title_col, 1);
    printf("%sPAC-MAN%s", CLR_TITLE, CLR_RESET);

    move_cursor(OFFSET_X + MAP_W * CELL_W - 18, 1);
    printf("%sHI-SCORE%s", CLR_DIM, CLR_RESET);
    move_cursor(OFFSET_X + MAP_W * CELL_W - 8, 1);
    printf("%s%8d%s", CLR_SCORE, m->high_score, CLR_RESET);

    /* Linha 2: separador + info de nivel */
    move_cursor(OFFSET_X, 2);
    printf("%s", CLR_DIM);
    for (int i = 0; i < MAP_W * CELL_W; i++) printf("─");
    printf("%s", CLR_RESET);

    /* Linha 3: vidas e nivel */
    move_cursor(OFFSET_X, 3);
    printf("                                                        ");
    move_cursor(OFFSET_X, 3);
    printf("%sVIDAS:%s ", CLR_DIM, CLR_RESET);
    for (int i = 0; i < m->lives - 1 && i < 5; i++)
        printf("%sᗧ %s", CLR_PACMAN, CLR_RESET);

    /* Auto-play badge */
    if (m->auto_play) {
        move_cursor(OFFSET_X + MAP_W * CELL_W / 2 - 6, 3);
        printf("%s[ AUTO-PLAY ]%s", CLR_AUTO_TAG, CLR_RESET);
    }

    move_cursor(OFFSET_X + MAP_W * CELL_W - 24, 3);
    printf("%sFASE:%2d%s", CLR_MAGENTA, m->level, CLR_RESET);
    move_cursor(OFFSET_X + MAP_W * CELL_W - 12, 3);
    printf("%sLV %2d%s", CLR_CYAN, m->level, CLR_RESET);

    move_cursor(OFFSET_X + MAP_W * CELL_W - 28, 4);
    printf("%sMAPA:%s %s%s%s", CLR_GREEN, CLR_RESET,
           (m->map_variant == 1) ? CLR_MAGENTA : CLR_CYAN,
           (m->map_variant == 1) ? "SHADOW" : "CLASS",
           CLR_RESET);
    move_cursor(OFFSET_X + MAP_W * CELL_W - 12, 4);
    printf("%sDIF:%s %s%s%s", CLR_ORANGE, CLR_RESET,
           (m->difficulty == 2) ? CLR_GAMEOVER : (m->difficulty == 1) ? CLR_TITLE : CLR_CYAN,
           (m->difficulty == 2) ? "DIFICIL" : (m->difficulty == 1) ? "MEDIO" : "FACIL",
           CLR_RESET);
}

/* ======================== MAPA ======================== */

void view_draw_cell(GameModel *m, int x, int y) {
    int tile = m->grid[y][x];
    switch (tile) {
        case TILE_WALL:
            move_cursor(OFFSET_X + x * CELL_W, OFFSET_Y + y);
            if (g_use_emoji) printf("%s🟦 %s", CLR_WALL, CLR_RESET);
            else            printf("%s██%s", CLR_WALL, CLR_RESET);
            break;
        case TILE_PELLET:
            move_cursor(OFFSET_X + x * CELL_W, OFFSET_Y + y);
            if (g_use_emoji) printf("%s• %s", CLR_PELLET, CLR_RESET);
            else            printf("%s··%s", CLR_PELLET, CLR_RESET);
            break;
        case TILE_POWER:
            move_cursor(OFFSET_X + x * CELL_W, OFFSET_Y + y);
            if (m->frame_count % 20 < 10)
                if (g_use_emoji) printf("%s🍒 %s", CLR_POWER, CLR_RESET);
                else            printf("%s✦✦%s", CLR_POWER, CLR_RESET);
            else
                printf("  ");
            break;
        case TILE_DOOR:
            move_cursor(OFFSET_X + x * CELL_W, OFFSET_Y + y);
            if (g_use_emoji) printf("%s⬜ %s", CLR_DOOR, CLR_RESET);
            else            printf("%s▂▂%s", CLR_DOOR, CLR_RESET);
            break;
        default:
            move_cursor(OFFSET_X + x * CELL_W, OFFSET_Y + y);
            printf("  ");
            break;
    }
}

void view_draw_map(GameModel *m) {
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++)
            view_draw_cell(m, x, y);
    view_draw_map_border();
}

/* ======================== ENTIDADES ======================== */

static const char *pacman_sprite(GameModel *m) {
    if (g_use_emoji) {
        /* Emoji costuma ser maior e já “vende” a ideia, sem direção */
        return ((m->frame_count / 3) % 2 == 0) ? "😋 " : "⚪ ";
    }
    int anim = (m->frame_count / 3) % 4;
    int dx = m->current_dx, dy = m->current_dy;
    if (dx == 0 && dy == 0) dx = 1;
    if (anim == 0 || anim == 2) {
        if (dx > 0) return "ᗧ ";
        if (dx < 0) return " ᗤ";
        if (dy < 0) return "ᗢ ";
        return "ᗣ ";
    }
    return "● ";
}

static const char *ghost_sprite(Ghost *g, int frame) {
    if (g_use_emoji) {
        if (g->vulnerable) return (frame % 6 < 3) ? "🥶 " : "👻 ";
        return "👻 ";
    }
    if (g->vulnerable) {
        /* Pisca quando prestes a acabar o poder */
        return (frame % 6 < 3) ? "ᗣ " : "◌ ";
    }
    return "ᗣ ";
}

static const char *ghost_color(int i, Ghost *g, int frame) {
    if (g->vulnerable) {
        return (frame % 8 < 4) ? CLR_VULN : CLR_VULN2;
    }
    switch (i) {
        case 0: return CLR_BLINKY;
        case 1: return CLR_PINKY;
        case 2: return CLR_INKY;
        case 3: return CLR_CLYDE;
    }
    return CLR_RESET;
}

void view_draw_entities(GameModel *m) {
    for (int i = 0; i < GHOSTS; i++) {
        if (m->ghosts[i].in_house && m->ghosts[i].release_timer > 0) continue;
        Ghost *g = &m->ghosts[i];
        move_cursor(OFFSET_X + g->e.x * CELL_W, OFFSET_Y + g->e.y);
        printf("%s%s%s", ghost_color(i, g, m->frame_count),
               ghost_sprite(g, m->frame_count), CLR_RESET);
    }
    /* Pac-Man por cima */
    move_cursor(OFFSET_X + m->pacman.x * CELL_W, OFFSET_Y + m->pacman.y);
    printf("%s%s%s", CLR_PACMAN, pacman_sprite(m), CLR_RESET);
}

void view_erase_entities(GameModel *m) {
    view_draw_cell(m, m->pacman.x, m->pacman.y);
    for (int i = 0; i < GHOSTS; i++) {
        if (m->ghosts[i].in_house && m->ghosts[i].release_timer > 0) continue;
        view_draw_cell(m, m->ghosts[i].e.x, m->ghosts[i].e.y);
    }
}

/* ======================== PAINEL LATERAL ======================== */

/* Coluna X do painel lateral */
#define PANEL_X  (OFFSET_X + MAP_W * CELL_W + 3)
#define PANEL_W  18

static void panel_line(int row, const char *label, const char *val_color, const char *val) {
    int px = PANEL_X;
    int py = row;
    move_cursor(px, py);
    printf("%s│%s %-9s %s%-6s%s %s│%s",
           CLR_PANEL_BORDER, CLR_RESET,
           label,
           val_color, val, CLR_RESET,
           CLR_PANEL_BORDER, CLR_RESET);
}

static void panel_separator(int row) {
    move_cursor(PANEL_X, row);
    printf("%s├──────────────────┤%s", CLR_PANEL_BORDER, CLR_RESET);
}

static void panel_header(int row, const char *text) {
    move_cursor(PANEL_X, row);
    printf("%s│%s %s%-16s%s %s│%s",
           CLR_PANEL_BORDER, CLR_RESET,
           CLR_PANEL_HDR, text, CLR_RESET,
           CLR_PANEL_BORDER, CLR_RESET);
}

void view_draw_side_panel(GameModel *m) {
    int r = OFFSET_Y;

    /* Topo */
    move_cursor(PANEL_X, r++);
    printf("%s╭──────────────────╮%s", CLR_PANEL_BORDER, CLR_RESET);

    panel_header(r++, "  ESTRUTURAS");
    panel_separator(r++);

    /* BST */
    {
        char buf[12];
        snprintf(buf, sizeof(buf), "h=%d n=%d",
                 bst_height(m->score_tree),
                 bst_count(m->score_tree));
        panel_line(r++, "BST", CLR_CYAN, buf);
    }

    /* AVL */
    {
        char buf[12];
        snprintf(buf, sizeof(buf), "h=%d b=%d",
                 avl_height(m->powerup_tree),
                 avl_balance_factor(m->powerup_tree));
        panel_line(r++, "AVL", CLR_CYAN, buf);
    }

    /* Grafo */
    {
        int edges = 0;
        if (m->maze_graph) {
            for (int i = 0; i < m->maze_graph->num_vertices; i++) {
                AdjNode *cur = m->maze_graph->adj_list[i];
                while (cur) { edges++; cur = cur->next; }
            }
        }
        char buf[12];
        snprintf(buf, sizeof(buf), "E=%d", edges);
        panel_line(r++, "Grafo", CLR_CYAN, buf);
    }

    {
        char buf[12];
        snprintf(buf, sizeof(buf), "%s", (m->map_variant == 1) ? "SHADOW" : "CLASS");
        panel_line(r++, "Mapa", CLR_MAGENTA, buf);
    }

    {
        const char *diff_label = "FACIL";
        if (m->difficulty == 1) diff_label = "MEDIO";
        else if (m->difficulty == 2) diff_label = "DIFICIL";
        panel_line(r++, "Dific.", CLR_ORANGE, diff_label);
    }

    {
        char buf[12] = "Nenhum";
        if (m->shield_timer > 0) strncpy(buf, "Escudo", sizeof(buf));
        else if (m->speed_timer > 0) strncpy(buf, "Turbo", sizeof(buf));
        else if (m->ghost_slow_timer > 0) strncpy(buf, "Lento", sizeof(buf));
        else if (m->time_freeze_timer > 0) strncpy(buf, "Congela", sizeof(buf));
        else if (m->double_score_timer > 0) strncpy(buf, "x2 Pts", sizeof(buf));
        panel_line(r++, "Poder", CLR_GREEN, buf);
    }

    panel_separator(r++);
    panel_header(r++, "  GHOST  AI");
    panel_separator(r++);

    {
        char b[10];
        snprintf(b, sizeof(b), "Dijkstra");
        panel_line(r++, "Blinky", CLR_BLINKY, b);
        snprintf(b, sizeof(b), "BFS");
        panel_line(r++, "Pinky", CLR_PINKY, b);
        snprintf(b, sizeof(b), "DFS");
        panel_line(r++, "Inky", CLR_INKY, b);
        snprintf(b, sizeof(b), "Manhatt.");
        panel_line(r++, "Clyde", CLR_CLYDE, b);
    }

    panel_separator(r++);
    panel_header(r++, "  JOGO");
    panel_separator(r++);

    /* Power timer */
    {
        char buf[12];
        if (m->power_timer > 0)
            snprintf(buf, sizeof(buf), "%3ds", m->power_timer / 12);
        else
            snprintf(buf, sizeof(buf), "--");
        panel_line(r++, "POWER", CLR_POWER, buf);
    }

    /* Pellets */
    {
        char buf[12];
        snprintf(buf, sizeof(buf), "%d", m->pellets_left);
        panel_line(r++, "Dots", CLR_PELLET, buf);
    }

    /* Combo */
    {
        char buf[12];
        snprintf(buf, sizeof(buf), "x%d", m->ghost_eat_combo);
        panel_line(r++, "Combo", CLR_ORANGE, buf);
    }

    panel_separator(r++);

    /* Auto-play status */
    if (m->auto_play) {
        panel_header(r++, "  AUTO-PLAY");
        panel_separator(r++);
        {
            char buf[12];
            snprintf(buf, sizeof(buf), "%d", m->auto_pellets_eaten);
            panel_line(r++, "Pellets", CLR_AUTO_TAG, buf);
        }
        {
            char buf[12];
            snprintf(buf, sizeof(buf), "%d", m->auto_ghosts_eaten);
            panel_line(r++, "Ghosts", CLR_AUTO_TAG, buf);
        }
        /* Status resumido (até 8 chars) */
        {
            char status_short[10];
            strncpy(status_short, m->auto_status, 8);
            status_short[8] = '\0';
            panel_line(r++, "Modo", CLR_GREEN, status_short);
        }
        panel_separator(r++);
    }

    /* Fundo */
    move_cursor(PANEL_X, r);
    printf("%s╰──────────────────╯%s", CLR_PANEL_BORDER, CLR_RESET);
}

/* ======================== HUD COMPLETO ======================== */

void view_draw_hud(GameModel *m) {
    view_draw_header(m);
    view_draw_side_panel(m);
}

/* ======================== MENSAGENS NO MAPA ======================== */

void view_draw_ready(void) {
    /* Centralizado na linha 17 do mapa */
    int col = OFFSET_X + (MAP_W * CELL_W / 2) - 4;
    move_cursor(col, OFFSET_Y + 17);
    printf("%s READY! %s", CLR_READY, CLR_RESET);
}

void view_clear_ready(void) {
    int col = OFFSET_X + (MAP_W * CELL_W / 2) - 4;
    move_cursor(col, OFFSET_Y + 17);
    printf("        ");
}

void view_draw_death_anim(GameModel *m, int frame) {
    const char *frames[] = {"ᗧ ", "◔ ", "◑ ", "◕ ", "● ", "· ", "  "};
    int idx = frame / 2;
    if (idx >= 7) idx = 6;
    move_cursor(OFFSET_X + m->pacman.x * CELL_W, OFFSET_Y + m->pacman.y);
    printf("%s%s%s", CLR_PACMAN, frames[idx], CLR_RESET);
}

void view_draw_bonus(GameModel *m) {
    if (m->bonus_timer <= 0 || m->bonus_points <= 0) return;
    int sx = OFFSET_X + m->bonus_x * CELL_W;
    int sy = OFFSET_Y + m->bonus_y;
    move_cursor(sx, sy);
    printf("%s+%d%s", CLR_CYAN, m->bonus_points, CLR_RESET);
}

/* ======================== TELA COMPLETA DO JOGO ======================== */

void view_draw_game(GameModel *m) {
    view_clear();
    view_draw_map(m);
    view_draw_hud(m);
    view_draw_entities(m);
    view_flush();
}

/* ======================== MENU PRINCIPAL ======================== */

/* Desenha uma caixa centrada na tela */
static void draw_box(int col, int row, int w, int h) {
    move_cursor(col, row);
    printf("%s╔", CLR_WALL);
    for (int i = 0; i < w - 2; i++) printf("═");
    printf("╗%s", CLR_RESET);

    for (int r = row + 1; r < row + h - 1; r++) {
        move_cursor(col, r);
        printf("%s║%s", CLR_WALL, CLR_RESET);
        move_cursor(col + w - 1, r);
        printf("%s║%s", CLR_WALL, CLR_RESET);
    }

    move_cursor(col, row + h - 1);
    printf("%s╚", CLR_WALL);
    for (int i = 0; i < w - 2; i++) printf("═");
    printf("╝%s", CLR_RESET);
}

/* Caixa "grossa": borda dupla (externa + interna) */
static void draw_box_thick(int col, int row, int w, int h) {
    /* externa */
    move_cursor(col, row);
    printf("%s╔", CLR_WALL);
    for (int i = 0; i < w - 2; i++) printf("═");
    printf("╗%s", CLR_RESET);

    for (int r = row + 1; r < row + h - 1; r++) {
        move_cursor(col, r);
        printf("%s║%s", CLR_WALL, CLR_RESET);
        move_cursor(col + w - 1, r);
        printf("%s║%s", CLR_WALL, CLR_RESET);
    }

    move_cursor(col, row + h - 1);
    printf("%s╚", CLR_WALL);
    for (int i = 0; i < w - 2; i++) printf("═");
    printf("╝%s", CLR_RESET);

    /* interna (1 char pra dentro) */
    if (w < 6 || h < 6) return;
    int ic = col + 1, ir = row + 1, iw = w - 2, ih = h - 2;
    move_cursor(ic, ir);
    printf("%s╔", CLR_PANEL_BORDER);
    for (int i = 0; i < iw - 2; i++) printf("═");
    printf("╗%s", CLR_RESET);
    for (int r = ir + 1; r < ir + ih - 1; r++) {
        move_cursor(ic, r);
        printf("%s║%s", CLR_PANEL_BORDER, CLR_RESET);
        move_cursor(ic + iw - 1, r);
        printf("%s║%s", CLR_PANEL_BORDER, CLR_RESET);
    }
    move_cursor(ic, ir + ih - 1);
    printf("%s╚", CLR_PANEL_BORDER);
    for (int i = 0; i < iw - 2; i++) printf("═");
    printf("╝%s", CLR_RESET);
}

static void draw_divider(int col, int row, int w) {
    move_cursor(col, row);
    printf("%s╠", CLR_WALL);
    for (int i = 0; i < w - 2; i++) printf("═");
    printf("╣%s", CLR_RESET);
}

static void menu_item(int x, int y, int selected, const char *text, const char *hotkey) {
    move_cursor(x, y);
    if (selected) {
        printf("%s▶ %s%s%s", CLR_TITLE, text, CLR_DIM, hotkey ? hotkey : "");
        printf("%s", CLR_RESET);
    } else {
        printf("%s  %s%s%s", CLR_DIM, text, CLR_DIM, hotkey ? hotkey : "");
        printf("%s", CLR_RESET);
    }
}

void view_draw_menu(GameModel *m) {
    static int last_blink = -1;
    static int last_anim = -1;

    int bx = 3, by = 1;
    int bw = 62, bh = 38;
    draw_box_thick(bx, by, bw, bh);

    /* ── Banner ASCII art ── */
    int cy = by + 2;
    print_abs(bx + 6, cy,   CLR_TITLE, "██████╗  █████╗  ██████╗      ███╗   ███╗ █████╗ ███╗   ██╗");
    print_abs(bx + 6, cy+1, CLR_TITLE, "██╔══██╗██╔══██╗██╔════╝      ████╗ ████║██╔══██╗████╗  ██║");
    print_abs(bx + 6, cy+2, CLR_TITLE, "██████╔╝███████║██║  ███╗     ██╔████╔██║███████║██╔██╗ ██║");
    print_abs(bx + 6, cy+3, CLR_TITLE, "██╔═══╝ ██╔══██║██║   ██║     ██║╚██╔╝██║██╔══██║██║╚██╗██║");
    print_abs(bx + 6, cy+4, CLR_TITLE, "██║     ██║  ██║╚██████╔╝     ██║ ╚═╝ ██║██║  ██║██║ ╚████║");
    print_abs(bx + 6, cy+5, CLR_TITLE, "╚═╝     ╚═╝  ╚═╝ ╚═════╝      ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝");

    cy += 7;
    print_abs(bx + 13, cy, CLR_DIM, "Console Edition — Estruturas de Dados (UniCesumar)");

    /* Linha de "status" (versao/mode) */
    cy += 2;
    draw_divider(bx, cy, bw);

    /* Pequena animação (sem reposicionar o layout) */
    {
        int ax = bx + 6;
        int ay = cy + 1;
        int w = bw - 12;
        int t = (m->frame_count / 2) % (w - 8);
        if (t != last_anim) {
            last_anim = t;
            move_cursor(ax, ay);
            printf("%s", CLR_DIM);
            for (int i = 0; i < w; i++) printf(" ");
            printf("%s", CLR_RESET);
            move_cursor(ax + t, ay);
            if (g_use_emoji) printf("%s😋 %s", CLR_PACMAN, CLR_RESET);
            else printf("%sᗧ %s", CLR_PACMAN, CLR_RESET);
            move_cursor(ax + w - 4, ay);
            if (g_use_emoji) printf("%s👻%s", CLR_DIM, CLR_RESET);
            else printf("%sᗣ%s", CLR_DIM, CLR_RESET);
        }
    }

    /* ── Fantasmas ── */
    cy += 3;
    print_abs(bx + 4, cy, CLR_GREEN, "PERSONAGENS & IA");

    cy += 1;
    if (g_use_emoji) {
        print_abs(bx + 4,  cy, CLR_BLINKY, "  👻  BLINKY");
        print_abs(bx + 20, cy, CLR_BLINKY, "Dijkstra");
        print_abs(bx + 33, cy, CLR_PINKY,  "  👻  PINKY");
        print_abs(bx + 49, cy, CLR_PINKY,  "BFS");
    } else {
        print_abs(bx + 4,  cy, CLR_BLINKY, "  ᗣ  BLINKY");
        print_abs(bx + 19, cy, CLR_BLINKY, "Dijkstra");
        print_abs(bx + 33, cy, CLR_PINKY,  "  ᗣ  PINKY");
        print_abs(bx + 47, cy, CLR_PINKY,  "BFS");
    }

    cy += 1;
    if (g_use_emoji) {
        print_abs(bx + 4,  cy, CLR_INKY,  "  👻  INKY");
        print_abs(bx + 20, cy, CLR_INKY,  "DFS");
        print_abs(bx + 33, cy, CLR_CLYDE, "  👻  CLYDE");
        print_abs(bx + 49, cy, CLR_CLYDE, "Manhattan");
    } else {
        print_abs(bx + 4,  cy, CLR_INKY,  "  ᗣ  INKY");
        print_abs(bx + 19, cy, CLR_INKY,  "DFS");
        print_abs(bx + 33, cy, CLR_CLYDE, "  ᗣ  CLYDE");
        print_abs(bx + 47, cy, CLR_CLYDE, "Manhattan");
    }

    cy += 2;
    draw_divider(bx, cy, bw);

    /* ── Pontuacao ── */
    cy += 1;
    if (g_use_emoji) {
        print_abs(bx + 4, cy, CLR_PELLET, " •");
        print_abs(bx + 7, cy, CLR_DIM,    "= 10 pts");
        print_abs(bx + 20, cy, CLR_POWER, " 🍒");
        print_abs(bx + 24, cy, CLR_DIM,   "= 50 pts  power mode");
    } else {
        print_abs(bx + 4, cy, CLR_PELLET, " ·");
        print_abs(bx + 7, cy, CLR_DIM,    "= 10 pts");
        print_abs(bx + 20, cy, CLR_POWER, " ●");
        print_abs(bx + 23, cy, CLR_DIM,   "= 50 pts  power mode");
    }
    print_abs(bx + 46, cy, CLR_DIM,   "HIGH:");
    print_abs(bx + 52, cy, CLR_TITLE, "");
    move_cursor(bx + 52, cy);
    printf("%s%d%s", CLR_TITLE, m->high_score, CLR_RESET);

    cy += 2;
    draw_divider(bx, cy, bw);

    /* ── Controles ── */
    cy += 1;
    print_abs(bx + 4, cy, CLR_CYAN, "CONTROLES");

    cy += 1;
    print_abs(bx + 4,  cy, CLR_CYAN, "W A S D");
    print_abs(bx + 13, cy, CLR_DIM,  "ou  ← ↑ ↓ →   mover");
    print_abs(bx + 4,  cy+1, CLR_CYAN, "P");
    print_abs(bx + 13, cy+1, CLR_DIM,  "pausar / continuar");
    print_abs(bx + 4,  cy+2, CLR_CYAN, "Q");
    print_abs(bx + 13, cy+2, CLR_DIM,  "sair");

    cy += 4;
    draw_divider(bx, cy, bw);

    /* ── Menu de opcoes ── */
    cy += 1;
    print_abs(bx + 4, cy, CLR_GREEN, "INICIAR");

    cy += 2;
    {
        static const char *difficulty_labels[DIFFICULTY_LEVELS] = {"FACIL", "MEDIO", "DIFICIL"};
        static const char *map_labels[2] = {"CLASS", "SHADOW"};
        char difficulty_text[32];
        char map_text[32];

        snprintf(difficulty_text, sizeof(difficulty_text), "Dificuldade: %s", difficulty_labels[m->difficulty]);
        snprintf(map_text, sizeof(map_text), "Mapa: %s", map_labels[m->map_variant]);

        menu_item(bx + 12, cy + 0, m->menu_selected == 0, "Jogar (manual)", "   [ENTER]");
        menu_item(bx + 12, cy + 1, m->menu_selected == 1, "Jogar sozinho (auto-play)", "   [A]");
        menu_item(bx + 12, cy + 2, m->menu_selected == 2, difficulty_text, " ←/→");
        menu_item(bx + 12, cy + 3, m->menu_selected == 3, map_text, " ←/→");
        menu_item(bx + 12, cy + 4, m->menu_selected == 4, "Modo explicacao (como ganhar)", "   [E]");
        menu_item(bx + 12, cy + 5, m->menu_selected == 5, "High Scores", "   [H]");
        menu_item(bx + 12, cy + 6, m->menu_selected == 6, "Sair", "   [Q]");
    }

    cy += 7;
    /* Piscando */
    {
        int blink = (m->frame_count / 15) % 2;
        if (blink != last_blink) {
            last_blink = blink;
            if (blink == 0)
                print_abs(bx + 14, cy, CLR_SUBTITLE, "[ Use ↑/↓/←/→ e ENTER — ou WASD ]");
            else
                print_abs(bx + 14, cy, "", "                              ");
        }
    }

    cy += 2;
    if (g_use_emoji)
        print_abs(bx + 12, cy, CLR_DIM, "Modo emoji ativo. Desative com: PACMAN_EMOJI=0");
    else
        print_abs(bx + 8, cy, CLR_DIM, "Dica: PACMAN_EMOJI=1 ativa sprites em emoji (maiores).");

    view_flush();
    m->frame_count++;
}

/* ======================== GAME OVER ======================== */

void view_draw_gameover(GameModel *m) {
    /* Caixa sobreposta ao centro do mapa */
    int cx = OFFSET_X + 4, cy = OFFSET_Y + 11;
    int w = 46, h = 10;
    draw_box_thick(cx, cy, w, h);

    /* Titulo */
    move_cursor(cx + 3, cy + 2);
    if (g_use_emoji) printf("%s☠  GAME OVER%s", CLR_GAMEOVER, CLR_RESET);
    else printf("%sGAME  OVER%s", CLR_GAMEOVER, CLR_RESET);

    move_cursor(cx + 4, cy + 4);
    printf("%sSCORE:%s  %s%d%s", CLR_DIM, CLR_RESET, CLR_SCORE, m->score, CLR_RESET);

    move_cursor(cx + 4, cy + 5);
    printf("%sHI:   %s  %s%d%s", CLR_DIM, CLR_RESET, CLR_CYAN, m->high_score, CLR_RESET);

    /* Linha de feedback */
    move_cursor(cx + 4, cy + 6);
    printf("%sDica:%s %sH%s para ver ranking, ou %sENTER%s para menu.",
           CLR_DIM, CLR_RESET, CLR_CYAN, CLR_RESET, CLR_CYAN, CLR_RESET);

    move_cursor(cx + 4, cy + 7);
    printf("%sENTER=Menu   H=Scores   Q=Sair%s", CLR_DIM, CLR_RESET);

    view_flush();
}

/* ======================== VITORIA ======================== */

void view_draw_win(GameModel *m) {
    /* Flash das paredes */
    for (int f = 0; f < 8; f++) {
        const char *clr = (f % 2) ? CLR_TITLE : CLR_WALL;
        for (int y = 0; y < MAP_H; y++)
            for (int x = 0; x < MAP_W; x++)
                if (m->grid[y][x] == TILE_WALL) {
                    move_cursor(OFFSET_X + x * CELL_W, OFFSET_Y + y);
                    printf("%s▓▓%s", clr, CLR_RESET);
                }
        view_flush();
        sleep_ms(200);
    }

    int cx = OFFSET_X + 4, cy = OFFSET_Y + 11;
    int w = 46, h = 8;
    draw_box(cx, cy, w, h);

    move_cursor(cx + w/2 - 9, cy + 2);
    printf("%sLABIRINTO COMPLETO!%s", CLR_WIN_CLR, CLR_RESET);

    move_cursor(cx + 4, cy + 4);
    printf("%sSCORE:%s %s%d%s", CLR_DIM, CLR_RESET, CLR_TITLE, m->score, CLR_RESET);

    move_cursor(cx + 4, cy + 5);
    printf("%sENTER para continuar%s", CLR_DIM, CLR_RESET);

    view_flush();
}

/* ======================== HIGH SCORES ======================== */

void view_draw_scores(GameModel *m) {
    view_clear();

    int cx = 6, cy = 1;
    int w = 54, h = 30;
    draw_box_thick(cx, cy, w, h);

    /* Titulo */
    move_cursor(cx + w/2 - 12, cy + 1);
    printf("%s  HIGH SCORES — BST RANKING  %s", CLR_TITLE, CLR_RESET);

    draw_divider(cx, cy + 2, w);

    /* Cabecalho da tabela */
    move_cursor(cx + 2, cy + 3);
    printf("%s  #    NOME      PONTOS      GRADE%s", CLR_DIM, CLR_RESET);

    draw_divider(cx, cy + 4, w);

    /* Extrai scores da BST em ordem decrescente */
    int scores[20];
    char names[20][16];
    int count = 0;
    bst_inorder_rev(m->score_tree, scores, names, &count, 10);

    /* Demonstra Quick Sort */
    int sorted[20];
    memcpy(sorted, scores, count * sizeof(int));
    if (count > 1) quick_sort(sorted, 0, count - 1);

    /* Grades estilosas */
    const char *grades[] = {"S+", "S", "A+", "A", "B+", "B", "C+", "C", "D", "F"};
    const char *grade_colors[] = {
        CLR_TITLE, CLR_TITLE, CLR_GREEN, CLR_GREEN,
        CLR_CYAN, CLR_CYAN, CLR_MAGENTA, CLR_MAGENTA,
        CLR_ORANGE, CLR_GAMEOVER
    };

    for (int i = 0; i < 10; i++) {
        move_cursor(cx + 2, cy + 5 + i);
        if (i < count) {
            const char *num_clr = CLR_DIM;
            if (i == 0) num_clr = CLR_TITLE;
            else if (i == 1) num_clr = CLR_CYAN;
            else if (i == 2) num_clr = CLR_GREEN;

            const char *medal = "  ";
            if (i == 0) medal = "🥇";
            else if (i == 1) medal = "🥈";
            else if (i == 2) medal = "🥉";

            printf("%s║%s %s%s%2d.%s  %-8s  %s%8d%s pts  %s%2s%s  %s║%s",
                   CLR_WALL, CLR_RESET,
                   medal,
                   num_clr, i + 1, CLR_RESET,
                   names[i],
                   num_clr, scores[i], CLR_RESET,
                   grade_colors[i], grades[i], CLR_RESET,
                   CLR_WALL, CLR_RESET);
        } else {
            printf("%s║%s     %2d.  %-8s       ----        -   %s║%s",
                   CLR_WALL, CLR_RESET,
                   i + 1, "---",
                   CLR_WALL, CLR_RESET);
        }
    }

    draw_divider(cx, cy + 15, w);

    /* Info das estruturas */
    move_cursor(cx + 2, cy + 16);
    printf("%s║%s  %sBST%s h=%-2d n=%-2d    %sAVL%s h=%-2d fator=%d%s    %s║%s",
           CLR_WALL, CLR_RESET,
           CLR_CYAN, CLR_RESET,
           bst_height(m->score_tree), bst_count(m->score_tree),
           CLR_CYAN, CLR_RESET,
           avl_height(m->powerup_tree),
           avl_balance_factor(m->powerup_tree),
           CLR_RESET,
           CLR_WALL, CLR_RESET);

    move_cursor(cx + 2, cy + 17);
    printf("%s║%s  %sOrdenacao usada:%s Quick Sort (O n log n)%s            %s║%s",
           CLR_WALL, CLR_RESET, CLR_CYAN, CLR_RESET, CLR_RESET, CLR_WALL, CLR_RESET);

    draw_divider(cx, cy + 18, w);

    /* Todos os sorts */
    move_cursor(cx + 2, cy + 19);
    printf("%s║%s  %s7 Algoritmos disponíveis:%s%s                         %s║%s",
           CLR_WALL, CLR_RESET, CLR_GREEN, CLR_RESET, CLR_RESET, CLR_WALL, CLR_RESET);

    move_cursor(cx + 2, cy + 20);
    printf("%s║%s  Bubble  Selection  Insertion  Shell%s               %s║%s",
           CLR_WALL, CLR_RESET, CLR_RESET, CLR_WALL, CLR_RESET);

    move_cursor(cx + 2, cy + 21);
    printf("%s║%s  Merge   Quick      Heap%s                           %s║%s",
           CLR_WALL, CLR_RESET, CLR_RESET, CLR_WALL, CLR_RESET);

    draw_divider(cx, cy + 22, w);

    move_cursor(cx + 2, cy + 23);
    printf("%s║%s       %sPressione ENTER para voltar ao menu%s     %s║%s",
           CLR_WALL, CLR_RESET, CLR_DIM, CLR_RESET, CLR_WALL, CLR_RESET);

    view_flush();
}

/* ======================== MODO EXPLICACAO ======================== */

void view_draw_help(GameModel *m) {
    (void)m;
    view_clear();

    int bx = 4, by = 2;
    int bw = 60, bh = 28;
    draw_box_thick(bx, by, bw, bh);

    move_cursor(bx + 2, by + 1);
    printf("%sMODO EXPLICACAO%s", CLR_TITLE, CLR_RESET);

    draw_divider(bx, by + 2, bw);

    int r = by + 4;
    move_cursor(bx + 2, r++);
    printf("%sOBJETIVO%s", CLR_GREEN, CLR_RESET);
    move_cursor(bx + 2, r++);
    printf("%s- Coma todos os pontos (.%s/%s•%s) do labirinto.%s",
           CLR_DIM, CLR_RESET, CLR_PELLET, CLR_DIM, CLR_RESET);
    move_cursor(bx + 2, r++);
    printf("%s- Ao zerar os pontos, voce vence o nivel e passa para o proximo.%s", CLR_DIM, CLR_RESET);

    r++;
    move_cursor(bx + 2, r++);
    printf("%sFRUTA/POWER (o / 🍒)%s", CLR_GREEN, CLR_RESET);
    move_cursor(bx + 2, r++);
    printf("%s- Vale 50 pontos e ativa POWER por alguns segundos.%s", CLR_DIM, CLR_RESET);
    move_cursor(bx + 2, r++);
    printf("%s- Durante POWER, os fantasmas ficam vulneraveis e voce pode come-los.%s", CLR_DIM, CLR_RESET);
    move_cursor(bx + 2, r++);
    printf("%s- Poderes extras: Escudo, Turbo, Fantasmas Lentos, Congelar e x2 pontos.%s", CLR_DIM, CLR_RESET);

    r++;
    move_cursor(bx + 2, r++);
    printf("%sCONTROLES%s", CLR_GREEN, CLR_RESET);
    move_cursor(bx + 2, r++);
    printf("%s- Movimento: setas ou WASD%s", CLR_DIM, CLR_RESET);
    move_cursor(bx + 2, r++);
    printf("%s- P: pausa   Q: sair%s", CLR_DIM, CLR_RESET);

    r++;
    move_cursor(bx + 2, r++);
    printf("%sAUTO-PLAY%s", CLR_GREEN, CLR_RESET);
    move_cursor(bx + 2, r++);
    printf("%s- Usa Simulated Annealing (SA): planeja varios passos e evita riscos.%s", CLR_DIM, CLR_RESET);
    move_cursor(bx + 2, r++);
    printf("%s- Em POWER, muda para HUNT para priorizar comer fantasmas vulneraveis.%s", CLR_DIM, CLR_RESET);

    draw_divider(bx, by + bh - 3, bw);
    move_cursor(bx + 2, by + bh - 2);
    printf("%sENTER/Esc: voltar ao menu%s", CLR_DIM, CLR_RESET);

    view_flush();
}
