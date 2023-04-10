init:
    $ e = Character('Eileen')

label start:
    e "I'm gonna jump!"
    if True:
        "I have jumped"
    else:
        "I have not jumped"
    e "... Ouch"
    e "I'm gonna survive.. right?"
    if False:
        "I have survived"
    else
        "I have not survived"
        jump death
    
    "A new day is waiting"


label death:

    "Eileen have died"