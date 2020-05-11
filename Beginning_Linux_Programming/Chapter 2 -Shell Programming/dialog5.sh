if [ "$Q_MUSIC" = "1" ]; then
dialog --title "Likes Classical" --msgbox "Good choice!" 12 25
else
dialog --title "Doesn’t like Classical" --msgbox "Shame" 12 25
fi
