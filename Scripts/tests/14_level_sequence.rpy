define aug = Character('Augustina', color="#c8ffc8")

# Images
# image intro = "trial_intro"
transition intro = "trial_intro"

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

    show Augustina uniform with fade
    # uniform replaces dress, neutral replaces happy
    aug "Is this <blue>uniform</>! ... SO COOL RIGHT? ... why are you <red>blushing</>?"
