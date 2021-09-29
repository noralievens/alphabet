# Alphabet

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

<p align="center"> <img src="screenshot.png" alt="screenshot" width="400"/> </p>

[manual](doc/alphabet.md)

### installation
    all         compile and link
    man         convert doc/alphabet.md to manpage
    doxy        build doxygen documentation
    install     install in /usr/local/bin/alphabet
    uninstall   uninstall
    deb         builb .deb package
    appimg      builb .AppImage package
    app         build .app macos bundle
    apt         install dependencies with apt
    brew        install dependencies with homebrew
    clean       clean build, bin, doxy, man
    help        print this message
