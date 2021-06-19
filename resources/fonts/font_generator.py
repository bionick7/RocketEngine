import pygame

"""
drawline(screen, .5, 0, .5, 1)
drawline(screen, 0, .5, 1, .5)

drawline(screen, .95, 0, .95, 1)
drawline(screen, .05, 0, .05, 1)
drawline(screen, 0, .95, 1, .95)
drawline(screen, 0, .05, 1, .05)

drawline(screen, .2, 0, .2, 1)
drawline(screen, .8, 0, .8, 1)
drawline(screen, .2, .05, .8, .95)
drawline(screen, .8, .05, .2, .95)

drawthick(screen, 0, .33, 1, .33)
drawthick(screen, 0, .66, 1, .66)
"""

SNAP_POINTS = (

    (0.05, .05),
    (0.2, .05),
    (.5, .05),
    (0.8, .05),
    (0.95, .05),

    (0.05, .33),
    (0.2, .33),
    (.5, .33),
    (0.8, .33),
    (0.95, .33),

    (0.05, .5),
    (0.2, .5),
    (.5, .5),
    (0.8, .5),
    (0.95, .5),

    (0.05, .66),
    (0.2, .66),
    (.5, .66),
    (0.8, .66),
    (0.95, .66),

    (0.05, .95),
    (0.2, .95),
    (.5, .95),
    (0.8, .95),
    (0.95, .95),

    (0.385, .33),
    (0.615, .33),
    (0.39, .66),
    (0.61, .66),
)


def load():
    list_ = [[] for _ in range(127 - 32)]
    index = 0

    with open("def.vecfont", "rb") as f:
        content = f.read()
        for b in content:
            if b == 0x00:
                index += 1
            else:
                if index >= 32:
                    list_[index - 32].append(int(b - 32))

    print(list_)

    for i, c in enumerate(list_):
        list_[i] = [0.9 * n + 0.05 * 223 for n in c]

    index = 0
    return list_


def save(list_):
    list_2 = [[] for _ in range(127 - 32)]
    for i, c in enumerate(list_):
        list_2[i] = [min(int((n - 0.05 * 223) / 0.9), 223) for n in c]
    with open("def.vecfont", "wb") as f:
        f.write(b"\x00"*32)
        for character in list_2:
            f.write(bytearray([c + 32 for c in character] + [0]))


def quit():
    pygame.quit()
    exit()


WIDTH = 600
HEIGHT = 800
FONT_WIDTH = 223
FONT_HEIGHT = 223
LINE_COLOR = (200, 10, 10)


def main():
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))

    shift = False
    
    index = 0
    list_ = load()
    temp_list = list_[index]

    clock = pygame.time.Clock()
    while True:
        screen.fill((255, 255, 255))
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                list_[index] = temp_list
                save(list_)
                quit()
            elif event.type == pygame.MOUSEBUTTONUP:
                pos = list(pygame.mouse.get_pos())
                pos[0] = int(FONT_WIDTH * pos[0] / WIDTH)
                pos[1] = FONT_HEIGHT - int(FONT_HEIGHT * pos[1] / HEIGHT)
                print(pos)
                if shift and len(temp_list) % 4 != 0:
                    if abs(pos[0] - temp_list[-2]) > abs(pos[1] - temp_list[-1]):
                        pos[1] = temp_list[-1]
                    else:
                        pos[0] = temp_list[-2]
                temp_list.append(pos[0])
                temp_list.append(pos[1])
            elif event.type == pygame.KEYUP:
                if event.key == pygame.K_RIGHT:
                    list_[index] = temp_list
                    index = min(index + 1, 127-32)
                    print(index + 32, chr(index + 32))
                    temp_list = list_[index]
                elif event.key == pygame.K_LSHIFT or event.key == pygame.K_RSHIFT:
                    shift = False
                elif event.key == pygame.K_LEFT:
                    list_[index] = temp_list
                    index = max(index - 1, 0)
                    print(index + 32, chr(index + 32))
                    temp_list = list_[index]
                elif event.key == pygame.K_KP_ENTER:
                    list_[index] = temp_list
                elif event.key == pygame.K_BACKSPACE:
                    list_[index].clear()
                list_[index] = temp_list
                save(list_)
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LSHIFT or event.key == pygame.K_RSHIFT:
                    shift = True
        
        i = 0
        while i < len(temp_list) - 3:
            pygame.draw.line(screen, (0, 0, 0), 
                (temp_list[i] * WIDTH / FONT_WIDTH, (1 - temp_list[i+1] / FONT_HEIGHT) * HEIGHT), 
                (temp_list[i+2] * WIDTH / FONT_WIDTH, (1 - temp_list[i+3] / FONT_HEIGHT) * HEIGHT), 
            3)
            i += 4

        # Help lines
        drawline(screen, .5, 0, .5, 1)
        drawline(screen, 0, .5, 1, .5)
        
        drawline(screen, .95, 0, .95, 1)
        drawline(screen, .05, 0, .05, 1)
        drawline(screen, 0, .95, 1, .95)
        drawline(screen, 0, .05, 1, .05)
        
        drawline(screen, .2, 0, .2, 1)
        drawline(screen, .8, 0, .8, 1)
        drawline(screen, .2, .05, .8, .95)
        drawline(screen, .8, .05, .2, .95)

        drawthick(screen, 0, .33, 1, .33)
        drawthick(screen, 0, .66, 1, .66)

        for p in SNAP_POINTS:
            drawproximity(screen, *p, REGION / 2, REGION / 2)

        clock.tick(60)
        pygame.display.flip()


def snap(d):
    list_ = load()
    print(list_)
    for i, c in enumerate(list_):
        #print(c)
        for j in range(0, len(c), 2):
            px = c[j] / FONT_WIDTH
            py = c[j+1] / FONT_HEIGHT
            for snp in SNAP_POINTS:
                if dist_sqr(snp, (px, py)) < d * d:
                    list_[i][j] = int(snp[0] * FONT_WIDTH)
                    list_[i][j+1] = int(snp[1] * FONT_HEIGHT)
    print(list_)
    save(list_)


def resize():
    list_ = load()
    print(list_)
    for i, c in enumerate(list_):
        for j in range(0, len(c), 4):
            if c[j] > c[j+2]:
                list_[i][j+2], list_[i][j] = c[j], c[j+2]
            if c[j+1] > c[j+3]:
                list_[i][j+3], list_[i][j+1] = c[j+1], c[j+3]
    print(list_)
    save(list_)


def dist_sqr(p1, p2):
    dx = p1[0] - p2[0]
    dy = p1[1] - p2[1]
    return dx*dx + dy*dy


def drawline(screen, x_start, y_start, x_end, y_end):
    pygame.draw.line(screen, LINE_COLOR, (WIDTH * x_start, HEIGHT * y_start), (WIDTH * x_end, HEIGHT * y_end), 1)


def drawthick(screen, x_start, y_start, x_end, y_end):
    pygame.draw.line(screen, LINE_COLOR, (WIDTH * x_start, HEIGHT * y_start), (WIDTH * x_end, HEIGHT * y_end), 2)


def drawproximity(screen, x, y, a, b):
    pygame.draw.ellipse(screen, LINE_COLOR, pygame.Rect(WIDTH * (x - a), HEIGHT * (y - b), 2 * a * WIDTH, 2 * b * HEIGHT), 1)


REGION = 0.03

if __name__ == "__main__":
    # resize()
    main()
    # snap(REGION)
