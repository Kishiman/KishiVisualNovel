define s = Character('Sylvie', color="#c8ffc8")
define m = Character('Me', color="#c8c8ff")
image logo = "renpy logo.png"
image eileen happy = "eileen_happy_blue_dress.png"

label start:

    scene bg meadow

    "After a short while, we reach the meadows just outside the neighborhood where we both live."

    "It's a scenic view I've grown used to. Autumn is especially beautiful here."

    "When we were children, we played in these meadows a lot, so they're full of memories."

    m "Hey... Umm..."

    show sylvie green smile

    "She turns to me and smiles. She looks so welcoming that I feel my nervousness melt away."

    "I'll ask her...!"

    m "Ummm... Will you..."

    m "Will you be my artist for a visual novel?"

    show sylvie green surprised

    "Silence."

    jump leaving

label leaving:

    s "I'll get right on it!"

    hide sylvie

    "..."

    m "That wasn't what I meant!"
