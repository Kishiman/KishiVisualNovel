# The game starts here.

label start:

    stop music fadeout 2.0
    scene black with Pause (2.0)
    play sound alarm
    "*Ring ring ring.*"
    u "..."
    play sound alarm
    "*Ring ring ring.*"
    u "...?"
    th "W-what is that sound…?"
    play sound alarm 
    "*Ring ring ring.*"
    scene personalroom with d
    th "I opened my heavy eyes to see an unfamiliar room."
    th "Guess I was asleep. But where? This isn't my room…"
    th "I turned my head the right to see a small, old-fashioned alarm clock on the nightstand. The time read 7:30."
    u "7:30…? In the morning or evening?"
    th "I reached over to stop the alarm when suddenly I felt a dull pain in the back of my head."
    play sound exclaim with flash
    u "Ngh...?!"

    th """I sat up from the bed to rub my head and shut off the alarm, and it was then did I really take in the surroundings.

    The room gave off an ambiance of comfort. The lighting was calming against the warm colored walls.

    The furniture was devoid of any dust or dirt and the bed itself was extremely plush. It was overall a rather luxurious room without seeming overly elegant in its design.

    The only odd thing about it was that there didn't seem to be any windows. There were curtains, but they were drawn open against a wall featuring a single unassuming painting."""

    u "The room is comfortable enough but how on Earth did I get here…?"
    scene black with d
    th """I closed my eyes and furrowed my brow.

    Think back. Remember who you are and what you were doing last.

    …

    Ah, that's right.

    My name is {color=D7B213}Echo Pine.{/color}"""
    show ep with d

    th """The last thing I remember is seeing my best friend Ashley off for the day as she went to take care of her horses.

    Ashley… I wonder if she made it home alright. I admit I was a little less thorough in making sure she made it into her house that late afternoon but I'm sure she's alright.

    I hope…

    I wracked my brain, thinking about that evening."""
    hide ep with d

    # This ends the game.
    return
