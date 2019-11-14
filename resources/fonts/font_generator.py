import pygame


list_ = [[] for _ in range(127 - 32)]
index = 0

with open("def.vecfont", "rb") as f:
    content = f.read()
    for b in content:
        if b == 0x00:
            index += 1
        else:
            if index >= 32:
                list_[index - 32].append(int(b - 1))

index = 0
temp_list = list_[index]


def save():
    list_[index] = temp_list
    with open("def.vecfont", "wb") as f:
        f.write(b"\x00"*32)
        for character in list_:
            f.write(bytearray([c + 1 for c in character] + [0]))


def quit():
    save()
    pygame.quit()
    exit()

pygame.init()
screen = pygame.display.set_mode((254, 254))

shift = False

clock = pygame.time.Clock()
while True:
    screen.fill((255, 255, 255))
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            quit()
        elif event.type == pygame.MOUSEBUTTONUP:
            pos = list(pygame.mouse.get_pos())
            pos[1] = 254 - pos[1]
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
            save()
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_LSHIFT or event.key == pygame.K_RSHIFT:
                shift = True
    
    i = 0
    while i < len(temp_list) - 3:
        pygame.draw.line(screen, (0, 0, 0), (temp_list[i], 254-temp_list[i+1]), (temp_list[i+2], 254-temp_list[i+3]), 3)
        i += 4

    pygame.draw.line(screen, (200, 10, 10), (127, 0), (127, 255), 1)
    pygame.draw.line(screen, (200, 10, 10), (0, 127), (255, 127), 1)

    clock.tick(60)
    pygame.display.flip()


if __name__ == "__main__":
    main()
