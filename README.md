# Alphabet

nasty oneliner for otools

for file in $(otool -L bin/alphabet); do echo $file | grep "^/" | grep -v "/usr/lib\|/System" | xargs -I{} cp {} testotool/ ; done
