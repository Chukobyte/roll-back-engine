from rbe_py_api import *


class InputBuffer:
    """
    Takes action names as parameters to query for input
    """

    def __init__(self, move_left_action_name: str, move_right_action_name: str):
        self._move_left = move_left_action_name
        self._move_right = move_right_action_name

        self.move_left_pressed = False
        self.move_right_pressed = False

    def process_inputs(self) -> bool:
        self.move_left_pressed = Input.is_action_pressed(name=self._move_left)
        self.move_right_pressed = Input.is_action_pressed(name=self._move_right)
        return self.move_left_pressed or self.move_right_pressed

    def kill_inputs(self) -> None:
        self.move_left_pressed = False
        self.move_right_pressed = False


class NetworkSenderInputBuffer(InputBuffer):
    def __init__(self, move_left_action_name: str, move_right_action_name: str):
        super().__init__(move_left_action_name, move_right_action_name)
        self.is_server = True
        self.send_func = Server.send

    def process_inputs(self) -> bool:
        result = super().process_inputs()
        self.send_func(f"lm:{self.move_left_pressed},rm:{self.move_right_pressed}")
        return result


# TODO: Make receiver use prediction
class NetworkReceiverInputBuffer(InputBuffer):
    def process_inputs(self) -> bool:
        return False


# TODO: Implement AI
class AIInputBuffer(InputBuffer):
    def process_inputs(self) -> bool:
        return False
