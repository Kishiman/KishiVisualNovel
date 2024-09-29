define aug = Character('Augustina', color="#c8ffc8")

label start:
    show Augustina
    # displaying dress and neutral
    aug "I like this dress."
    
    hide Augustina
    aug "I like this dress."

    show Augustina happy
    # auto-defined in the auto group
    aug "But what I like even more..."

    show Augustina uniform happy
    # uniform replaces dress, neutral replaces happy
    aug "Is this uniform !"
