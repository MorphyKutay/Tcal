Output demo-en.gif

Set Shell "bash"
Set FontSize 13
Set Width 1000
Set Height 750
Set Theme "Dracula"
Set Padding 10

Type "TCAL_LANG=en ./tcal"
Enter
Sleep 1s

# navigate
Type "lll"
Sleep 600ms
Type "hh"
Sleep 600ms

# add a task: :e
Type ":e"
Enter
Sleep 500ms
Type "Dentist appointment"
Sleep 300ms
Enter
Sleep 800ms

Type ":e"
Enter
Sleep 500ms
Type "Submit report"
Sleep 300ms
Enter
Sleep 1s

# enter task selection (with j)
Type "j"
Sleep 600ms
Type "j"
Sleep 600ms
Type "k"
Sleep 600ms

# change month
Type "n"
Sleep 1s
Type "p"
Sleep 1s

# delete the selected task: :d
Type "j"
Sleep 500ms
Type ":d"
Enter
Sleep 1s

# quit
Type ":q"
Enter
Sleep 500ms
