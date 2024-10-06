define aug = Character('Augustina', color="#c8ffc8")

label start:
    show Augustina with fade
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

    show Augustina uniform happy with fade
    # uniform replaces dress, neutral replaces happy
    aug "Is this uniform !"
