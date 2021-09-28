# NAME
alphabet - music player

# SYNOPSIS
**alphabet** \[files...\]

# DESCRIPTION
Alphabet is a simple gtk-3 music player.\
It's mainly designed to allow precise comparison between similar music files (A-B testing).\
All audio files are matched to the track with lowest average loudness according to ebu R128.\
Playback is continuous so the user can switch files without interruption.\
A marker can be placed in the timeline to reset the playback position on file-selection or play/pause.\
Alternatively, the user can loop a specified region.\
Files can be opened in alphabet (file chooser), from the file manager (must use
.app bundle on MacOs), command-line arguments or they can be drag-and-dropped into alphabet (only in linux for now).\
A waveform showing the sort-term loudness over a 400ms window is displayed in
the timeline.\
Tracks can be sorted or manually sorted.

# OPTIONS
currently no options

# BUGS
wip

# COPYRIGHT
Copyright © 2021 Arno Lievens. License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\
This is free software: you are free to change and redistribute it.  There  is  NO
WARRANTY, to the extent permitted by law.
