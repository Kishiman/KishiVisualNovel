"This is narration."

"Eileen" "This is dialogue, with an explicit character name."

e "This is dialogue, using a character object instead."

"Bam!!" with vpunch
define e = Character("Eileen", image="eileen")

label start:

    show eileen mad
    e "I'm a little upset at you."

    e happy "But it's just a passing thing."

define e = Character("Eileen")

label start:

    show eileen
    e mad "I'm a little upset at you."

    e happy "That's funny."

    e -happy "I'm not sure what to think now."

    e happy @ vhappy "Really! That changes everything."

    e @ right -mad "My anger is temporarily suspended..."

    e "HOWEVER !"

"""
This is the first line of narration. It's longer than the other two
lines, so it has to wrap.

This is the second line of narration.

This is the third line of narration.
"""

e """
This is the first line of dialogue. It's longer than the other two
lines, so it has to wrap.

This is the second line of dialogue.

This is the third line of dialogue.
"""
