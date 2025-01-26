define aug = Character('Augustina', color="#c8ffc8")

# Images
image intro = "trial_intro"

label start:
    scene intro
    show Augustina dress smile with fade
    # displaying dress and neutral
    aug "I like this dress."
    
    hide Augustina with fade
    aug "I like this dress."

    show Augustina happy with fade
    # auto-defined in the auto group
    aug "But what I like even more..."

    show Augustina happy
    # auto-defined in the auto group
    aug "Wait for it..."

    scene intro onlayer student2

    show Augustina uniform happy with dissolve
    # uniform replaces dress, neutral replaces happy
    aug "Is this uniform !"
