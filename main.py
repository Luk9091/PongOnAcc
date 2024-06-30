import pygame
import sys
import numpy as np
from time import sleep

from tcp_server import TcpServer
TCP_ADDRESS = "10.42.0.1"
# TCP_ADDRESS = "192.168.88.252"
TCP_PORT = 4242

class Drawable():
    def __init__(self, width, height, x, y, color):
        self.width = width
        self.height = height
        self.color = color

        self.surface = pygame.Surface((width, height), pygame.SRCALPHA, 32).convert_alpha()
        self.rect = self.surface.get_rect(x=x, y=y)

    def draw_on(self, surface: pygame.Surface):
        surface.blit(self.surface, self.rect)



class Board():
    def __init__(self, width, height):
        self.surface = pygame.display.set_mode((width, height), 0, 32)
        pygame.display.set_caption('Simple Pong')
        self.fps_clock = pygame.time.Clock()

        self.ball = Ball(40, width/2, height/2)
        self.racket = Racket(300, 40, width/2, height*0.9)
        self.judge = Judge()

        self.server = TcpServer(TCP_ADDRESS, TCP_PORT)

    def run(self):
        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.server.stop()
                    pygame.quit()
                    sys.exit()
            (x, y, z) = self.server.read()
            self.racket.move(self, pygame.Vector2(-x, -y))
            # (x, y) = pygame.mouse.get_rel()
            # self.racket.move(self, pygame.Vector2(x, y))
            self.ball.move(self, self.racket, self.judge)
            self.draw(
                self.judge,
                self.racket,
                self.ball
            )
            self.fps_clock.tick(30)

    def draw(self, *args: Drawable):
        background = (0x30, 0x30, 0x30)
        self.surface.fill(background)

        for drawable in args:
            drawable.draw_on(self.surface)

        pygame.display.update()

    def reset(self):
        self.judge.reset()
        self.ball.reset()


class Racket(Drawable):
    def __init__(self, width, height, x, y, color = (0x00, 0xFF, 0x00), speed = 10):
        super().__init__(width, height, x-width/2, y-height/2, color)
        pygame.draw.rect(self.surface, self.color, [0, 0, width, height])
        self.speed = pygame.Vector2(0, 0)
        self.max_speed = pygame.Vector2(speed, speed)

    def move(self, board: Board, acceleration: pygame.Vector2):
        self.rect.move_ip(acceleration.x*2, acceleration.y*2)
        self.speed = acceleration

        if self.rect.top < board.surface.get_height()*0.7:
            self.rect.top = board.surface.get_height()*0.7
        elif self.rect.bottom > board.surface.get_height():
            self.rect.bottom = board.surface.get_height()
        if self.rect.left < 0:
            self.rect.left = 0
        elif self.rect.right > board.surface.get_width():
            self.rect.right = board.surface.get_width()


class Judge(Drawable):
    def __init__(self):
        self.score = 0
        pygame.font.init()
        fontPath = pygame.font.match_font('arial')
        self.font = pygame.font.Font(fontPath, 128)

    def draw_on(self, surface: pygame.Surface):
        text = self.font.render(f'{self.score}', True, (0xDE, 0xDE, 0xE0))
        rect = text.get_rect()
        rect.center = surface.get_rect().center
        surface.blit(text, rect)


    def addPoint(self):
        self.score += 1

    def reset(self):
        self.score = 0



class Ball(Drawable):
    def __init__(self, radius, x, y, color = (0xFF, 0x00, 0x00), x_speed = 3, y_speed = -9):
        super().__init__(radius*2, radius*2, x-radius, y-radius, color)
        pygame.draw.ellipse(self.surface, self.color, [0, 0, radius*2, radius*2])
        self.speed = pygame.Vector2(x_speed, y_speed)
        self.default_speed = pygame.Vector2(abs(x_speed), abs(y_speed))
        self.x_start = x - radius
        self.y_start = y - radius
        self.getOut = True

    def bounce_x(self):
        self.speed.x *= -1
    def bounce_y(self):
        self.speed.y *= -1

    def reset(self):
        self.rect.centerx = self.x_start
        self.rect.centery = self.y_start
        self.speed.x = self.default_speed.x
        self.speed.y = -self.default_speed.y
        self.getOut = True

    def move(self, board: Board, racket: Racket, judge: Judge):
        self.rect.x += self.speed.x
        self.rect.y += self.speed.y

        if abs(self.speed.x) > self.default_speed.x: self.speed.x *= 0.99
        if self.speed.y < self.default_speed.y: self.speed.y += 0.1

        if self.rect.left < 0 or self.rect.right > board.surface.get_width():
            self.bounce_x()
        if self.rect.top < 0 and self.speed.y < 0:
            self.bounce_y()

        if self.rect.colliderect(racket.rect):
            if self.getOut:
                judge.addPoint()
                self.getOut = False
            # self.bounce_y()
            if (racket.speed.y >= 0):
                self.bounce_y()
            else:
                self.speed.y += racket.speed.y
            if (self.speed.y > 0):
                self.speed.y *= -1
            self.speed.x += racket.speed.x
        
        if (self.rect.centery <= board.surface.get_height()*0.65):
            self.getOut = True

        if self.rect.top > board.surface.get_height():
            print('Game Over')
            board.reset()



if __name__ == '__main__':
    pygame.init()
    board = Board(800, 1000)
    board.run()