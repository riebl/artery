GLOBAL_CONST = 1


class Constants:
    CONST = 2


class Echo:
    def __init__(self, value: int):
        self.value = value

    def ping(self, delta: int):
        return self.value + delta


def echo_kwargs(**kwargs: str):
    """ Function that echoes key-value args passed to it. """
    return kwargs
