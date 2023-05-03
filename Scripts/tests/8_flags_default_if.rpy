define s = Character('Sylvie', color="#c8ffc8")
define m = Character('Me', color="#c8c8ff")
default book = False

label start:
    s "Sure, but what's a \"visual novel?\""
    menu:
        "It's a videogame.":
            jump game

        "It's an interactive book.":
            jump book

label game:

    m "It's a kind of videogame you can play on your computer or a console."

    jump marry

label book:

    $ book = True

    m "It's like an interactive book that you can read on a computer or a console."

    jump marry

label marry:
    
    if book:

        "Our first game is based on one of Sylvie's ideas, but afterwards I get to come up with stories of my own, too."

    else:   

        "Sylvie helped with the script on our first video game."

    "And so, we become a visual novel creating duo."