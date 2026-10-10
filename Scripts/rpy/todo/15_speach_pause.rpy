define aug = Character('Augustina', color="#c8ffc8")

# Images
# image intro = "trial_intro"
transition intro = "trial_intro"

label start:
    # scene intro
    scene _ with intro
    show Augustina dress smile with fade
    # displaying dress and neutral
    aug "What?!{w=0.3} No,{w=0.1} that's not what I meant—{w=0.3} I promise!"

    aug "What?! No, that's not what I meant— I promise!"
