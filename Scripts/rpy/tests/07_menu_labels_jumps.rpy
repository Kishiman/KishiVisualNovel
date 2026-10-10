define s = Character('Sylvie', color="#c8ffc8")
define m = Character('Me', color="#c8c8ff")


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

    m "It's like an interactive book that you can read on a computer or a console."

    jump marry

label marry:
    m "will you marry me?"
    menu:
        "Yes":
            s "Consent!"

        "No":
            s "Objection!"

    "And so, we become a visual novel creating duo."