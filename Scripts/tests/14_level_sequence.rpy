define aug = Character('Augustina', color="#c8ffc8")

# Images
# image intro = "trial_intro"
transition intro = "trial_intro"
image Augustina = "Augustina"

label start:
    # scene intro
    scene _ with intro
    show Augustina dress smile with fade
    # displaying dress and neutral
    aug "...................... I like this <red>dress</>."

    show Augustina dress tongue sweat
    # auto-defined in the auto group
    aug "But what I like even more..."

    show Augustina dress unsure blush half up
    # auto-defined in the auto group
    aug "Augustina dress unsure blush half up"

    show Augustina dress unsure blush half oneup
    # auto-defined in the auto group
    aug "Augustina dress unsure blush half oneup"

    # scene intro onlayer student2
    scene _ with intro onlayer student3

    show Augustina jeans surprise with fade
    # jeans replaces dress, neutral replaces happy
    aug "Is this <blue>jeans</>! ... SO COOL RIGHT? ... don't <red>laugh</>!"
    show Augustina jeans drool bigblush cry
    aug "don't <red>laugh</>!... please..."
