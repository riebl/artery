import warnings


def warn_user():
    warnings.warn(
        'You have imported Artery Python Bindings outside of Omnet++'
        'runtime. Functionality of this module may be severly lacking.'
    )