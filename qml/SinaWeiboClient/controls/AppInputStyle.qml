import QtQuick 1.1
import com.nokia.meego 1.0
TextFieldStyle {
 TextFieldStyle {id:normal}
 textColor:settingModel.darkMode?"#edf2f7":normal.textColor
 promptTextColor:settingModel.darkMode?"#94a3b2":normal.promptTextColor
 selectedTextColor:settingModel.darkMode?"#edf2f7":normal.selectedTextColor
 selectionColor:settingModel.darkMode?"#375e7c":normal.selectionColor
 background:settingModel.darkMode?Qt.resolvedUrl("icons/dark/input.svg"):normal.background
 backgroundSelected:settingModel.darkMode?Qt.resolvedUrl("icons/dark/input-focus.svg"):normal.backgroundSelected
 backgroundDisabled:settingModel.darkMode?Qt.resolvedUrl("icons/dark/input-disabled.svg"):normal.backgroundDisabled
 backgroundError:settingModel.darkMode?Qt.resolvedUrl("icons/dark/input-error.svg"):normal.backgroundError
}
