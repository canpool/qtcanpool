/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
**/

import QtQuick 2.3
import QtQuick.Window 2.2

Rectangle
{
    visible: true
    focus: true

    color: "#303030"

    property string windowIcon
    property string windowIconGrayed
    property string windowTitle
    property bool isMaximized
    property bool isVisible
    property bool isActive
    property bool isTitleBarVisible

    function setButtonState(button, state)
    {
        switch (button)
        {
        case "minBtn":
            switch (state)
            {
            case "hover_enter":
                minBtn.hoverEnter()
                break;
            case "hover_leave":
                minBtn.hoverLeave()
                break;
            case "button_press":
                minBtn.buttonPress()
                break;
            case "button_release":
                minBtn.buttonRelease()
                break;
            default:
                break;
            }
            break;
        case "maxBtn":
            switch (state)
            {
            case "hover_enter":
                maxBtn.hoverEnter()
                break;
            case "hover_leave":
                maxBtn.hoverLeave()
                break;
            case "button_press":
                maxBtn.buttonPress()
                break;
            case "button_release":
                maxBtn.buttonRelease()
                break;
            default:
                break;
            }
            break;
        case "clsBtn":
            switch (state)
            {
            case "hover_enter":
                clsBtn.hoverEnter()
                break;
            case "hover_leave":
                clsBtn.hoverLeave()
                break;
            case "button_press":
                clsBtn.buttonPress()
                break;
            case "button_release":
                clsBtn.buttonRelease()
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    Column
    {
        anchors.fill: parent

        Rectangle
        {
            id: titlebar
            color: "#222222"
            width: parent.width
            height: 30
            z: 1
            visible: isTitleBarVisible

            Rectangle
            {
                id: windowIconRect
                x: 0
                y: 0
                width: 30
                height: parent.height
                color: "transparent"

                Image
                {
                    id: windowIconImage
                    anchors.centerIn: parent
                    width: 16
                    height: 16
                    smooth: true
                    source: isTitleBarVisible ? (isActive ? windowIcon : windowIconGrayed) : ""
                }
            }

            Text
            {
                id: windowTitleText
                x: windowIconRect.width
                y: 0
                anchors.left: windowIconRect.right
                anchors.right: minBtn.left
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Qt.AlignCenter
                color: isActive ? windowTitle.color = "#FFFFFF" : windowTitle.color = "#AAAAAA"
                elide: Text.ElideRight
                text: windowTitle
            }

            CaptionButton
            {
                id: minBtn
                x: parent.width - clsBtn.width - maxBtn.width - width
                y: 0
                width: parent.height
                height: parent.height
                buttonColor: "#009900"
                buttonColorPressed: "#005500"
                imgSource: isTitleBarVisible ? loadPixmapToString(":/res/minimize.svg") : ""
            }

            CaptionButton
            {
                id: maxBtn
                x: parent.width - clsBtn.width - width
                y: 0
                width: parent.height
                height: parent.height
                buttonColor: "#999900"
                buttonColorPressed: "#555500"
                imgSource: isTitleBarVisible ? (!isMaximized ? loadPixmapToString(":/res/maximize.svg") :
                                                               loadPixmapToString(":/res/restore.svg")) : ""
            }

            CaptionButton
            {
                id: clsBtn
                x: parent.width - width
                y: 0
                width: parent.height
                height: parent.height
                buttonColor: "#FF0000"
                buttonColorPressed: "#990000"
                imgSource: isTitleBarVisible ? loadPixmapToString(":/res/close.svg") : ""
            }
        }

        Rectangle
        {
            id: contents
            color: "transparent"
            y: (titlebar.visible ? titlebar.height : 0)
            width: parent.width
            height: parent.height - (titlebar.visible ? titlebar.height : 0)

            Rectangle
            {
                color: "green"
                width: Math.min(parent.width, parent.height) / 2
                height: Math.min(parent.width, parent.height) / 2
                anchors.centerIn: parent

                RotationAnimation on rotation
                {
                    loops: Animation.Infinite
                    duration: 1000
                    from: 0
                    to: 360
                }
            }
        }
    }
}
