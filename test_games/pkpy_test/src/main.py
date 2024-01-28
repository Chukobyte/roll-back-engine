from crescent import Node2D, Input, Engine, TextLabel, Vector2, Particles2D, Engine, MinMaxVector2


class Main(Node2D):
    def _start(self) -> None:
        # text_label = TextLabel.new()
        # text_label.text = "Hey"
        # text_label.position = Vector2(30, 30)
        # self.add_child(text_label)

        # particles2d = Particles2D.new()
        # particles2d.position = Vector2(300, 300)
        # particles2d.initial_velocity = MinMaxVector2(
        #     Vector2(-90, -60),
        #     Vector2(900, 600)
        # )
        # particles2d.explosiveness = 1.0
        # self.add_child(particles2d)

        Engine.set_fps_display_enabled(True)

    def _process(self, delta_time: float) -> None:
        if Input.is_action_just_pressed("exit"):
            Engine.exit()

        # particles2D = self.get_child("Particles2D")
        # if particles2D:
        #     particles2D.position += Vector2(1, 1)
        sprite = self.get_child("Sprite")
        if sprite:
            sprite.position += Vector2(100, 100) * Vector2(delta_time, delta_time)

    def _fixed_process(self, delta_time: float) -> None:
        pass
        # sprite = self.get_child("Sprite")
        # if sprite:
        #     sprite.position += Vector2(100, 100) * Vector2(delta_time, delta_time)
