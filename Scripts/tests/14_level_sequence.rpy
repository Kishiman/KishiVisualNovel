define aug = Character('Augustina', color="#c8ffc8")

# Images
# image intro = "trial_intro"
transition intro = "trial_intro"
image Augustina = "Augustina2/Augustina2"

label start:
    # scene intro
    scene _ with intro
    show Augustina dress smile with fade
    # displaying dress and neutral
    aug "...................... I like this <red>dress</>."

    # auto-defined in the auto group
    aug "But what I like even more..."

    # auto-defined in the auto group
    aug "Wait for it..."

    # scene intro onlayer student2
    scene _ with intro onlayer student3

    show Augustina jeans with fade
    # jeans replaces dress, neutral replaces happy
    aug "Is this <blue>jeans</>! ... SO COOL RIGHT? ... why are you <red>blushing</>?"
