dialog --title "Confirm" --yesno "Are you willing to take part?" 9 18
if [ $? != 0 ]; then
dialog --infobox "Thank you anyway" 5 20
sleep 2
dialog --clear
exit 0
fi
