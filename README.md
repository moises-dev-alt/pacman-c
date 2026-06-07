# 🎮 PAC-MAN Console — Projeto MVC em C

Um reimaginação clássica do Pac-Man em C, implementada com **padrão de arquitetura MVC (Model-View-Controller)** e integrada com **estruturas de dados e algoritmos acadêmicos**. O projeto executa totalmente em console/terminal com suporte multiplataforma (Windows, Linux, macOS).

---

## 📋 Sumário

1. [Características](#características)
2. [Como Rodar](#como-rodar)
3. [Estrutura do Projeto](#estrutura-do-projeto)
4. [Arquitetura MVC](#arquitetura-mvc)
5. [Estruturas de Dados Implementadas](#estruturas-de-dados-implementadas)
6. [Explicação do Código](#explicação-do-código)
7. [Controles do Jogo](#controles-do-jogo)
8. [Requisitos](#requisitos)

---

## ✨ Características

- 🕹️ **Gameplay Clássico**: Colete pellets, evite fantasmas, colete power-ups
- 🧠 **Auto-Play com IA**: Modo automático onde o Pac-Man joga sozinho usando BFS
- 🗺️ **Dois Mapas Diferentes**: Mapa clássico e mapa "Shadow"
- 📊 **Ranking de High Scores**: Sistema de pontuação persistente com BST
- 🎨 **Interface Visual Aprimorada**: Paleta de cores 256 ANSI com tema visual elegante
- ⚙️ **Três Níveis de Dificuldade**: Fácil, Médio, Difícil
- 💪 **Power-ups Diversos**:
  - **Power Pellets**: Torna fantasmas vulneráveis
  - **Shield**: Protege o Pac-Man
  - **Speed Boost**: Aumenta velocidade
  - **Slow Motion**: Desacelera fantasmas
  - **Time Freeze**: Congela fantasmas
  - **Double Points**: Duplica pontuação
- 🤖 **IAs de Fantasmas Sofisticadas**:
  - **Blinky**: Usa algoritmo de Dijkstra (caminho mais curto)
  - **Pinky**: Usa BFS (busca em largura)
  - **Inky**: Usa DFS (busca em profundidade)
  - **Clyde**: Comportamento aleatório
- 🖥️ **Multiplataforma**: Funciona em Windows, Linux e macOS

---

## 🚀 Como Rodar

### Pré-requisitos

- **GCC** ou outro compilador C (como `gcc`, `clang`)
- **Make** (opcional, mas recomendado)
- Terminal com suporte a cores ANSI (todos os terminais modernos suportam)

### Linux / macOS

```bash
# Navegar até o diretório do projeto
cd pacman

# Opção 1: Usar Makefile
make

# Opção 2: Compilar manualmente
gcc -I. -o pacman controller/controller.c model/model.c view/view.c -lm

# Executar
./pacman
```

### Windows

```bash
# Opção 1: MinGW com Make
cd pacman
mingw32-make

# Opção 2: GCC direto
gcc -I. -o pacman.exe controller/controller.c model/model.c view/view.c -lm

# Executar
pacman.exe
```

### Limpar arquivos compilados

```bash
make clean
```

---

## 📁 Estrutura do Projeto

```
pacman/
├── Makefile                 # Script de compilação
├── pacman.h                 # Header com definições globais e estruturas
├── controller/
│   └── controller.c         # Loop principal, input do usuário, auto-play IA
├── model/
│   └── model.c              # Lógica do jogo, estruturas de dados, algoritmos
└── view/
    └── view.c               # Renderização visual no console
```

### Arquivo Principal: `pacman.h`

Este header centraliza todas as definições globais do projeto:

```c
/* Dimensões do mapa */
#define MAP_W 28        /* Largura do mapa */
#define MAP_H 31        /* Altura do mapa */

/* Mecânica do jogo */
#define GHOSTS 4                /* 4 fantasmas (Blinky, Pinky, Inky, Clyde) */
#define FRAME_MS 85             /* Intervalo entre frames em ms */
#define START_LIVES 3           /* Vidas iniciais */
#define DIFFICULTY_LEVELS 3     /* Fácil, Médio, Difícil */

/* Estruturas de cores 256 ANSI */
#define CLR_WALL        "\033[38;5;57m"      /* Paredes em azul escuro */
#define CLR_PELLET      "\033[38;5;227m"    /* Pellets em amarelo */
#define CLR_PACMAN      "\033[38;5;220;1m"  /* Pac-Man em amarelo */
#define CLR_BLINKY      "\033[38;5;201;1m"  /* Blinky (vermelho) */
/* ... mais cores ... */
```

---

## 🏗️ Arquitetura MVC

O projeto segue o padrão **Model-View-Controller**, separando responsabilidades:

### 1️⃣ **MODEL** (`model/model.c`)

**Responsabilidade**: Lógica do jogo e processamento de dados

**Contém**:
- Estruturas de dados (mapa, posições do Pac-Man, fantasmas)
- Inicialização do jogo
- Atualização de estado (movimento, colisões, pontuação)
- Estruturas de dados acadêmicas (BST, AVL, Grafos)
- Algoritmos de busca (BFS, DFS, Dijkstra)

**Exemplo**:
```c
/* Estrutura principal do jogo */
typedef struct {
    int px, py;                 /* Posição Pac-Man */
    int gx[GHOSTS], gy[GHOSTS]; /* Posições dos fantasmas */
    int score, lives, level;
    TileType **map;             /* Mapa 2D */
    GameState state;
    /* ... mais campos ... */
} GameModel;

/* Função para atualizar o jogo a cada frame */
void model_update(GameModel *m, int delta_ms);
```

### 2️⃣ **VIEW** (`view/view.c`)

**Responsabilidade**: Renderização visual no console

**Contém**:
- Inicialização do terminal (cores, buffer)
- Rendering do mapa, Pac-Man, fantasmas
- Menu principal, tela de game over, rankings
- Painel lateral com informações
- Controle do cursor e limpeza de tela

**Exemplo**:
```c
/* Renderiza o estado atual do jogo */
void view_draw_game(GameModel *m);

/* Renderiza o menu principal */
void view_draw_menu(GameModel *m);

/* Limpa a tela do console */
void view_clear(void);
```

### 3️⃣ **CONTROLLER** (`controller/controller.c`)

**Responsabilidade**: Input do usuário e lógica do loop principal

**Contém**:
- Tratamento de input (teclado, setas)
- Loop principal do jogo
- Auto-Play (IA que controla o Pac-Man automaticamente)
- Sincronização de frames

**Exemplo**:
```c
/* Processa input do usuário */
void controller_handle_input(GameModel *m);

/* Loop principal do jogo */
void game_loop(GameModel *m);

/* Auto-Play: Pac-Man joga sozinho usando BFS */
void autoplay_move_pacman(GameModel *m);
```

### Fluxo de Execução

```
INICIO
  ↓
view_init()           ← Inicializa console (cores, cursor)
  ↓
model_init()          ← Inicializa estruturas do jogo
  ↓
╔═════════════════════════════════════════════════╗
║ GAME LOOP (enquanto jogo está rodando)          ║
║  1. controller_handle_input()  ← Lê teclado    ║
║  2. model_update()             ← Atualiza lógica║
║  3. view_draw_game()           ← Renderiza      ║
║  4. sleep(FRAME_MS)            ← Sincroniza FPS ║
╚═════════════════════════════════════════════════╝
  ↓
view_cleanup()        ← Restaura terminal original
  ↓
FIM
```

---

## 🧮 Estruturas de Dados Implementadas

### 1. **Árvore Binária de Busca (BST)** - High Scores

Armazena os top scores em ordem crescente/decrescente para rápida busca e inserção.

```c
typedef struct BSTNode {
    int score;
    char name[16];
    struct BSTNode *left, *right;
} BSTNode;

/* Operações principais: O(log n) em média */
BSTNode *bst_insert(BSTNode *root, int score, const char *name);
BSTNode *bst_search(BSTNode *root, int score);
BSTNode *bst_remove(BSTNode *root, int score);
```

**Uso no Projeto**:
- Armazena até 100 melhores scores
- Busca rápida de um score específico
- Inserção/remoção eficiente

---

### 2. **Árvore AVL** - Inventário de Power-ups

Árvore balanceada que mantém power-ups ativos ordenados por tempo de expiração.

```c
typedef struct AVLNode {
    int power_type;
    int time_remaining;
    struct AVLNode *left, *right;
    int height;
} AVLNode;

/* Operações balanceadas: O(log n) garantido */
AVLNode *avl_insert(AVLNode *root, int power_type, int duration);
void avl_update(AVLNode *root, int delta_ms);
```

**Uso no Projeto**:
- Rastreia múltiplos power-ups ativos simultaneamente
- Balanceamento automático garante operações rápidas
- Remoção eficiente quando expiram

---

### 3. **Grafos** - Representação do Labirinto

O mapa é representado como um grafo onde cada célula é um nó e cada célula adjacente é uma aresta.

```c
typedef struct {
    int *adj[MAP_W * MAP_H];  /* Lista de adjacência */
    int adj_count[MAP_W * MAP_H];
    int node_count;
} Graph;

/* Funções de grafo */
void graph_init(GameModel *m);
int graph_pos_to_id(int x, int y);
void graph_add_edge(Graph *g, int u, int v);
```

**Uso no Projeto**:
- Armazena a estrutura do labirinto
- Conecta células transitáveis
- Base para algoritmos de busca (BFS, DFS, Dijkstra)

---

### 4. **Busca em Profundidade (DFS)** - IA do Inky

Explora o grafo em profundidade para encontrar caminho até o Pac-Man.

```c
void dfs(int node, int target, int *parent, int *visited);

/* Usa DFS para calcular caminho mais profundo até o Pac-Man */
void ghost_inky_ai(GameModel *m, int ghost_id);
```

**Características**:
- Usa pilha (recursão) para exploração
- Tempo: O(V + E)
- Inky é imprevisível, segue caminhos alternativos

---

### 5. **Busca em Largura (BFS)** - IA do Pinky + Auto-Play

Busca o caminho **mais curto** usando fila.

```c
void bfs(int start, int target, int *parent, Graph *g);

/* Encontra pellet mais próximo não coletado */
void autoplay_bfs_find_pellet(GameModel *m);

/* Pinky usa BFS para chegar ao Pac-Man */
void ghost_pinky_ai(GameModel *m, int ghost_id);
```

**Características**:
- Usa fila para exploração por níveis
- Tempo: O(V + E)
- Garante o caminho mais curto
- **Auto-Play**: Pac-Man escolhe o pellet mais próximo e navega até ele

---

### 6. **Algoritmo de Dijkstra** - IA do Blinky

Encontra o caminho **de menor custo** (mais rápido) até o Pac-Man.

```c
void dijkstra(int start, int *dist, int *parent, Graph *g);

/* Blinky usa Dijkstra para alcançar Pac-Man otimamente */
void ghost_blinky_ai(GameModel *m, int ghost_id);
```

**Características**:
- Calcula distâncias mínimas de um nó para todos os outros
- Tempo: O((V + E) log V)
- Blinky é o fantasma mais perigoso

---

### 7. **Algoritmos de Ordenação** - Ranking de Scores

Implementações de 7 algoritmos para ordenar rankings:

```c
void bubble_sort(int *scores, int n);
void selection_sort(int *scores, int n);
void insertion_sort(int *scores, int n);
void shell_sort(int *scores, int n);
void merge_sort(int *scores, int left, int right);
void quick_sort(int *scores, int left, int right);
void heap_sort(int *scores, int n);
```

**Uso no Projeto**:
- Ordenação de scores para exibição do ranking
- Comparação de performance entre algoritmos

---

## 💻 Explicação do Código

### 1. Inicialização do Jogo

**`model.c`** - `model_init()`
```c
void model_init(GameModel *m, int map_variant) {
    m->map = (TileType **)malloc(MAP_H * sizeof(TileType *));
    for (int i = 0; i < MAP_H; i++) {
        m->map[i] = (TileType *)malloc(MAP_W * sizeof(TileType));
    }
    
    /* Carrega um dos dois mapas */
    const char *raw_map = (map_variant == 0) ? raw_map_classic : raw_map_shadow;
    
    /* Converte caracteres em tipos de tile */
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            char c = raw_map[y][x];
            if (c == '#') m->map[y][x] = TILE_WALL;
            else if (c == '.') m->map[y][x] = TILE_PELLET;
            else if (c == 'o') m->map[y][x] = TILE_POWER;
            else m->map[y][x] = TILE_EMPTY;
        }
    }
    
    /* Posiciona Pac-Man e fantasmas */
    m->px = 1; m->py = 1;
    m->gx[0] = 13; m->gy[0] = 14;  /* Blinky */
    m->gx[1] = 12; m->gy[1] = 15;  /* Pinky */
    /* ... mais fantasmas ... */
    
    /* Inicializa grafo para algoritmos */
    graph_init(m);
    
    /* Inicializa ranking de scores (BST) */
    m->score_tree = NULL;
}
```

---

### 2. Loop Principal

**`controller.c`** - `game_loop()`
```c
void game_loop(GameModel *m) {
    long last_time = current_time_ms();
    
    while (m->running) {
        long now = current_time_ms();
        int delta_ms = now - last_time;
        last_time = now;
        
        /* 1. ENTRADA: Processa teclado */
        if (m->auto_play) {
            autoplay_move_pacman(m);        /* IA joga */
        } else {
            controller_handle_input(m);     /* Jogador controla */
        }
        
        /* 2. ATUALIZAÇÃO: Processa lógica */
        model_update(m, delta_ms);
        
        /* 3. RENDERIZAÇÃO: Desenha na tela */
        view_draw_game(m);
        view_flush();
        
        /* 4. SINCRONIZAÇÃO: Mantém FPS constante */
        long frame_time = current_time_ms() - now;
        if (frame_time < FRAME_MS) {
            sleep_ms(FRAME_MS - frame_time);
        }
    }
}
```

---

### 3. Auto-Play com BFS

**`controller.c`** - `autoplay_move_pacman()`
```c
void autoplay_move_pacman(GameModel *m) {
    /* Se não há pellet alvo, encontra o mais próximo */
    if (m->auto_target_x < 0 || m->auto_target_y < 0) {
        autoplay_bfs_find_pellet(m);  /* BFS: acha pellet mais próximo */
    }
    
    /* Se ainda não achou pellet, Pac-Man fica parado */
    if (m->auto_target_x < 0) return;
    
    /* BFS: Encontra caminho até o pellet alvo */
    int parent[MAP_W * MAP_H];
    bfs(m->px * MAP_W + m->py, 
        m->auto_target_x * MAP_W + m->auto_target_y, 
        parent, &m->graph);
    
    /* Segue o caminho: próximo passo em direção ao pellet */
    int next_node = m->px * MAP_W + m->py;
    while (parent[next_node] != -1 && 
           parent[next_node] != (m->auto_target_x * MAP_W + m->auto_target_y)) {
        next_node = parent[next_node];
    }
    
    /* Converte volta para (x, y) */
    int next_x = next_node / MAP_H;
    int next_y = next_node % MAP_H;
    m->desired_dx = next_x - m->px;
    m->desired_dy = next_y - m->py;
    
    /* Se chegou no pellet, limpa o alvo */
    if (m->px == m->auto_target_x && m->py == m->auto_target_y) {
        m->auto_target_x = -1;
        m->auto_target_y = -1;
    }
}
```

---

### 4. Renderização do Mapa

**`view.c`** - `view_draw_game()`
```c
void view_draw_game(GameModel *m) {
    /* Renderiza o mapa */
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            move_cursor(x * CELL_W + OFFSET_X, y + OFFSET_Y);
            
            switch (m->map[y][x]) {
                case TILE_WALL:
                    printf("%s██%s", CLR_WALL, CLR_RESET);
                    break;
                case TILE_PELLET:
                    printf("%s·%s ", CLR_PELLET, CLR_RESET);
                    break;
                case TILE_POWER:
                    printf("%s●%s ", CLR_POWER, CLR_RESET);
                    break;
                default:
                    printf("   ");
            }
        }
    }
    
    /* Renderiza Pac-Man */
    move_cursor(m->px * CELL_W + OFFSET_X, m->py + OFFSET_Y);
    printf("%sC%s", CLR_PACMAN, CLR_RESET);
    
    /* Renderiza fantasmas */
    const char *ghost_colors[] = {CLR_BLINKY, CLR_PINKY, CLR_INKY, CLR_CLYDE};
    for (int i = 0; i < GHOSTS; i++) {
        move_cursor(m->gx[i] * CELL_W + OFFSET_X, m->gy[i] + OFFSET_Y);
        printf("%sG%s", ghost_colors[i], CLR_RESET);
    }
    
    /* HUD com score, vidas, nível */
    move_cursor(2, 2);
    printf("Score: %d | Vidas: %d | Nível: %d", 
           m->score, m->lives, m->level);
}
```

---

### 5. Detecção de Colisão

**`model.c`** - `model_update()`
```c
void model_update(GameModel *m, int delta_ms) {
    /* Movimento Pac-Man */
    int new_px = m->px + m->desired_dx;
    int new_py = m->py + m->desired_dy;
    
    /* Verifica se pode se mover (não é parede) */
    if (m->map[new_py][new_px] != TILE_WALL) {
        m->px = new_px;
        m->py = new_py;
    }
    
    /* Coleta pellets */
    if (m->map[m->py][m->px] == TILE_PELLET) {
        m->score += 10;
        m->map[m->py][m->px] = TILE_EMPTY;
        m->pellets_remaining--;
    }
    
    /* Coleta power-ups */
    if (m->map[m->py][m->px] == TILE_POWER) {
        m->score += 50;
        m->map[m->py][m->px] = TILE_EMPTY;
        m->power_active = 1;
        m->power_time = POWER_TIME;
    }
    
    /* Movimento e IA dos fantasmas */
    for (int i = 0; i < GHOSTS; i++) {
        /* Cada fantasma usa sua IA específica */
        if (i == 0) ghost_blinky_ai(m, i);      /* Dijkstra */
        else if (i == 1) ghost_pinky_ai(m, i);  /* BFS */
        else if (i == 2) ghost_inky_ai(m, i);   /* DFS */
        else ghost_clyde_ai(m, i);              /* Aleatório */
    }
    
    /* Colisão com fantasmas */
    for (int i = 0; i < GHOSTS; i++) {
        if (m->px == m->gx[i] && m->py == m->gy[i]) {
            if (m->power_active) {
                /* Pac-Man comeu o fantasma! */
                m->score += 200;
                m->gx[i] = 14; m->gy[i] = 14;  /* Reseta posição */
                m->power_active = 0;
            } else {
                /* Pac-Man foi pego */
                m->lives--;
                if (m->lives <= 0) m->state = STATE_GAMEOVER;
                else m->state = STATE_DYING;
            }
        }
    }
    
    /* Atualiza power-ups */
    if (m->power_active) {
        m->power_time -= delta_ms;
        if (m->power_time <= 0) {
            m->power_active = 0;
        }
    }
    
    /* Verifica vitória */
    if (m->pellets_remaining == 0) {
        m->state = STATE_WIN;
        m->level++;
    }
}
```

---

## 🕹️ Controles do Jogo

### Movimento
- **WASD** ou **Setas de Teclado**: Mover Pac-Man
- **P**: Pausar/Retomar
- **Q**: Sair

### Menu
- **Setas Para Cima/Baixo**: Navegar menu
- **Setas Esquerda/Direita**: Ajustar configurações (dificuldade, mapa)
- **ENTER**: Selecionar opção

### Modos de Jogo
1. **Play**: Você controla o Pac-Man
2. **Auto-Play**: IA controla (BFS para encontrar pellets)
3. **Difficulty**: Escolha entre Fácil, Médio, Difícil
4. **Map**: Alterne entre Clássico e Shadow
5. **High Scores**: Visualize os melhores scores
6. **Help**: Veja os controles
7. **Quit**: Sair

---

## 📋 Requisitos

### Compilação
- **GCC** (versão 5.0 ou superior)
- **Make** (opcional)
- **C99 ou C11** padrão

### Runtime
- Terminal com suporte a cores ANSI (praticamente todos os modernos)
- Windows 7+ (recomendado Windows 10/11 ou WSL2 para melhor experiência)
- Linux (qualquer distribuição recente)
- macOS

### Bibliotecas
- **libm** (Math library) - incluída automaticamente pelo GCC

---

## 🎯 Fluxo de Compreenção do Código

Para entender melhor o projeto, recomendo ler na seguinte ordem:

1. **`pacman.h`** - Entenda as constantes, enums e estruturas principais
2. **`model.c`** - Aprenda como o jogo funciona (estruturas de dados, algoritmos)
3. **`controller.c`** - Veja como o input e o auto-play funcionam
4. **`view.c`** - Veja como o visual é renderizado

---

## 🔧 Possíveis Extensões

- 📱 Modo multiplayer local
- 🎵 Efeitos sonoros
- 💾 Salvar/carregar partida
- 🌐 Ranking online
- 🤖 Mais modos de IA para fantasmas
- 🎮 Suporte a gamepad/joystick

---

## 📝 Notas Importantes

### Estruturas de Dados Educacionais

Este projeto foi desenvolvido para demonstrar a aplicação prática de:
- Estruturas de dados fundamentais (BST, AVL, Grafos)
- Algoritmos de busca (BFS, DFS, Dijkstra)
- Padrões de arquitetura de software (MVC)
- Programação em C puro (sem frameworks externos)

### Performance

- O projeto foi otimizado para rodar suavemente até em máquinas antigas
- Buffer de saída configurable para reduzir flicker
- Sincronização de frames para gameplay consistente
- Algoritmos com complexidade aceita para tempo real

---

## 📞 Suporte

Para dúvidas sobre:
- **Compilação**: Verifique se tem GCC instalado (`gcc --version`)
- **Cores ANSI**: Atualize seu terminal para uma versão recente
- **Performance**: Reduza a dificuldade ou feche outros programas

---

**Divirta-se jogando! 🎮**
