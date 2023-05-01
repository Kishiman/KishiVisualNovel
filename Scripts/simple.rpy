init:
    image bg whitehouse = "whitehouse.jpg"
    image eileen happy = "eileen_happy.png"
    image eileen upset = "eileen_upset.png"
    $ e = Character('Eileen')

label start:

    scene bg whitehouse
    show eileen happy

    e "I'm standing in front of the White House."

    show eileen upset

    e "I once wanted to go on a tour of the West Wing, but you have to 
        know somebody to get in."

    "For some reason, she really seems upset about this."

    e "I considered sneaking in, but that probably isn't a good idea."