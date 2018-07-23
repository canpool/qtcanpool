/***************************************************************************
**                                                                        **
**  QtWord, an easy to write Microsoft Word (doc/docx) file format        **
**  Copyright (C) 2017-2020 Canpool                                       **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Canpool (maminjie)                                   **
**  Website/Contact: canpool@163.com / QQ1074867723                       **
**             Date: 17.11.11                                             **
**          Version: 0.1.0                                                **
****************************************************************************/
#ifndef QTWORD_H
#define QTWORD_H

#include <QObject>
#include <QAxObject>
#include <QAxWidget>

// decl definitions for shared library compilation/usage:
#if defined(QTWORD_COMPILE_LIBRARY)
#  define QWD_LIB_DECL Q_DECL_EXPORT
#elif defined(QTWORD_USE_LIBRARY)
#  define QWD_LIB_DECL Q_DECL_IMPORT
#else
#  define QWD_LIB_DECL
#endif

namespace Word{

//WdUnits
enum WdUnits{
    wdCell = 12,//A cell.
    wdCharacter = 1,//A character.
    wdCharacterFormatting = 13,//Character formatting.
    wdColumn = 9,//A column.
    wdItem = 16,//The selected item.
    wdLine = 5,//A line.
    wdParagraph = 4,//A paragraph.
    wdParagraphFormatting = 14,//Paragraph formatting.
    wdRow = 10,//A row.
    wdScreen = 7,//The screen dimensions.
    wdSection = 8,//A section.
    wdSentence = 3,//A sentence.
    wdStory = 6,//A story.
    wdTable = 15,//A table.
    wdWindow = 11,//A window.
    wdWord = 2//A word.
};

enum WdMovementType{
    wdExtend = 1,//The end of the selection is extended to the end of the specified unit.
    wdMove = 0,//The selection is collapsed to an insertion point and moved to the end of the specified unit. Default.
};

//对齐方式
enum WdParagraphAlignment{
    wdAlignParagraphLeft=0,			//Left-aligned.
    wdAlignParagraphCenter=1,		//Center-aligned.
    wdAlignParagraphRight=2,		//Right-aligned.
    wdAlignParagraphJustify=3,		//Fully justified.
    wdAlignParagraphDistribute=4,	//Paragraph characters are distributed to fill the entire width of the paragraph.
    wdAlignParagraphJustifyMed=5,	//Justified with a medium character compression ratio.
    wdAlignParagraphJustifyHi=7,	//Justified with a high character compression ratio.
    wdAlignParagraphJustifyLow=8,	//Justified with a low character compression ratio.
    wdAlignParagraphThaiJustify=9	//Justified according to Thai formatting layout.
};

//页面方向
enum WdOrientation{
    wdOrientPortrait = 0, // 纵向
    wdOrientLandscape = 1 // 横向
};

//样式
enum WdBuiltinStyle{
    wdStyleBlockQuotation = -85,//Block Text.
    wdStyleBodyText = -67,//Body Text.
    wdStyleBodyText2 = -81,//Body Text 2.
    wdStyleBodyText3 = -82,//Body Text 3.
    wdStyleBodyTextFirstIndent = -78,//Body Text First Indent.
    wdStyleBodyTextFirstIndent2 = -79,//Body Text First Indent 2.
    wdStyleBodyTextIndent = -68,//Body Text Indent.
    wdStyleBodyTextIndent2 = -83,//Body Text Indent 2.
    wdStyleBodyTextIndent3 = -84,//Body Text Indent 3.
    wdStyleBookTitle = -265,//Book Title.
    wdStyleCaption = -35,//Caption.
    wdStyleClosing = -64,//Closing.
    wdStyleCommentReference = -40,//Comment Reference.
    wdStyleCommentText = -31,//Comment Text.
    wdStyleDate = -77,//Date.
    wdStyleDefaultParagraphFont = -66,//Default Paragraph Font.
    wdStyleEmphasis = -89,//Emphasis.
    wdStyleEndnoteReference = -43,//Endnote Reference.
    wdStyleEndnoteText = -44,//Endnote Text.
    wdStyleEnvelopeAddress = -37,//Envelope Address.
    wdStyleEnvelopeReturn = -38,//Envelope Return.
    wdStyleFooter = -33,//Footer.
    wdStyleFootnoteReference = -39,//Footnote Reference.
    wdStyleFootnoteText = -30,//Footnote Text.
    wdStyleHeader = -32,//Header.
    wdStyleHeading1 = -2,//Heading 1.
    wdStyleHeading2 = -3,//Heading 2.
    wdStyleHeading3 = -4,//Heading 3.
    wdStyleHeading4 = -5,//Heading 4.
    wdStyleHeading5 = -6,//Heading 5.
    wdStyleHeading6 = -7,//Heading 6.
    wdStyleHeading7 = -8,//Heading 7.
    wdStyleHeading8 = -9,//Heading 8.
    wdStyleHeading9 = -10,//Heading 9.
    wdStyleHtmlAcronym = -96,//HTML Acronym.
    wdStyleHtmlAddress = -97,//HTML Address.
    wdStyleHtmlCite = -98,//HTML Cite.
    wdStyleHtmlCode = -99,//HTML Code.
    wdStyleHtmlDfn = -100,//HTML Definition.
    wdStyleHtmlKbd = -101,//HTML Keyboard.
    wdStyleHtmlNormal = -95,//Normal (Web).
    wdStyleHtmlPre = -102,//HTML Preformatted.
    wdStyleHtmlSamp = -103,//HTML Sample.
    wdStyleHtmlTt = -104,//HTML Typewriter.
    wdStyleHtmlVar = -105,//HTML Variable.
    wdStyleHyperlink = -86,//Hyperlink.
    wdStyleHyperlinkFollowed = -87,//Followed Hyperlink.
    wdStyleIndex1 = -11,//Index 1.
    wdStyleIndex2 = -12,//Index 2.
    wdStyleIndex3 = -13,//Index 3.
    wdStyleIndex4 = -14,//Index 4.
    wdStyleIndex5 = -15,//Index 5.
    wdStyleIndex6 = -16,//Index 6.
    wdStyleIndex7 = -17,//Index 7.
    wdStyleIndex8 = -18,//Index 8.
    wdStyleIndex9 = -19,//Index 9.
    wdStyleIndexHeading = -34,//Index Heading
    wdStyleIntenseEmphasis = -262,//Intense Emphasis.
    wdStyleIntenseQuote = -182,//Intense Quote.
    wdStyleIntenseReference = -264,//Intense Reference.
    wdStyleLineNumber = -41,//Line Number.
    wdStyleList = -48,//List.
    wdStyleList2 = -51,//List 2.
    wdStyleList3 = -52,//List 3.
    wdStyleList4 = -53,//List 4.
    wdStyleList5 = -54,//List 5.
    wdStyleListBullet = -49,//List Bullet.
    wdStyleListBullet2 = -55,//List Bullet 2.
    wdStyleListBullet3 = -56,//List Bullet 3.
    wdStyleListBullet4 = -57,//List Bullet 4.
    wdStyleListBullet5 = -58,//List Bullet 5.
    wdStyleListContinue = -69,//List Continue.
    wdStyleListContinue2 = -70,//List Continue 2.
    wdStyleListContinue3 = -71,//List Continue 3.
    wdStyleListContinue4 = -72,//List Continue 4.
    wdStyleListContinue5 = -73,//List Continue 5.
    wdStyleListNumber = -50,//List Number.
    wdStyleListNumber2 = -59,//List Number 2.
    wdStyleListNumber3 = -60,//List Number 3.
    wdStyleListNumber4 = -61,//List Number 4.
    wdStyleListNumber5 = -62,//List Number 5.
    wdStyleListParagraph = -180,//List Paragraph.
    wdStyleMacroText = -46,//Macro Text.
    wdStyleMessageHeader = -74,//Message Header.
    wdStyleNavPane = -90,//Document Map.
    wdStyleNormal = -1,//Normal.
    wdStyleNormalIndent = -29,//Normal Indent.
    wdStyleNormalObject = -158,//Normal (applied to an object).
    wdStyleNormalTable = -106,//Normal (applied within a table).
    wdStyleNoteHeading = -80,//Note Heading.
    wdStylePageNumber = -42,//Page Number.
    wdStylePlainText = -91,//Plain Text.
    wdStyleQuote = -181,//Quote.
    wdStyleSalutation = -76,//Salutation.
    wdStyleSignature = -65,//Signature.
    wdStyleStrong = -88,//Strong.
    wdStyleSubtitle = -75,//Subtitle.
    wdStyleSubtleEmphasis = -261,//Subtle Emphasis.
    wdStyleSubtleReference = -263,//Subtle Reference.
    wdStyleTableColorfulGrid = -172,//Colorful Grid.
    wdStyleTableColorfulList = -171,//Colorful List.
    wdStyleTableColorfulShading = -170,//Colorful Shading.
    wdStyleTableDarkList = -169,//Dark List.
    wdStyleTableLightGrid = -161,//Light Grid.
    wdStyleTableLightGridAccent1 = -175,//Light Grid Accent 1.
    wdStyleTableLightList = -160,//Light List.
    wdStyleTableLightListAccent1 = -174,//Light List Accent 1.
    wdStyleTableLightShading = -159,//Light Shading.
    wdStyleTableLightShadingAccent1 = -173,//Light Shading Accent 1.
    wdStyleTableMediumGrid1 = -166,//Medium Grid 1.
    wdStyleTableMediumGrid2 = -167,//Medium Grid 2.
    wdStyleTableMediumGrid3 = -168,//Medium Grid 3.
    wdStyleTableMediumList1 = -164,//Medium List 1.
    wdStyleTableMediumList1Accent1 = -178,//Medium List 1 Accent 1.
    wdStyleTableMediumList2 = -165,//Medium List 2.
    wdStyleTableMediumShading1 = -162,//Medium Shading 1.
    wdStyleTableMediumShading1Accent1 = -176,//Medium Shading 1 Accent 1.
    wdStyleTableMediumShading2 = -163,//Medium Shading 2.
    wdStyleTableMediumShading2Accent1 = -177,//Medium Shading 2 Accent 1.
    wdStyleTableOfAuthorities = -45,//Table of Authorities.
    wdStyleTableOfFigures = -36,//Table of Figures.
    wdStyleTitle = -63,//Title.
    wdStyleTOAHeading = -47,//TOA Heading.
    wdStyleTOC1 = -20,//TOC 1.
    wdStyleTOC2 = -21,//TOC 2.
    wdStyleTOC3 = -22,//TOC 3.
    wdStyleTOC4 = -23,//TOC 4.
    wdStyleTOC5 = -24,//TOC 5.
    wdStyleTOC6 = -25,//TOC 6.
    wdStyleTOC7 = -26,//TOC 7.
    wdStyleTOC8 = -27,//TOC 8.
    wdStyleTOC9 = -28,//TOC 9.
};

//下划线
enum WdUnderline{
    wdUnderlineDash=7,				//Dashes.
    wdUnderlineDashHeavy=23,		//Heavy dashes.
    wdUnderlineDashLong=39,			//Long dashes.
    wdUnderlineDashLongHeavy=55,	//Long heavy dashes.
    wdUnderlineDotDash=9,			//Alternating dots and dashes.
    wdUnderlineDotDashHeavy=25,		//Alternating heavy dots and heavy dashes.
    wdUnderlineDotDotDash=10,		//An alternating dot-dot-dash pattern.
    wdUnderlineDotDotDashHeavy=26,	//An alternating heavy dot-dot-dash pattern.
    wdUnderlineDotted=4,			//Dots.
    wdUnderlineDottedHeavy=20,		//Heavy dots.
    wdUnderlineDouble=3,			//A double line.
    wdUnderlineNone=0,				//No underline.
    wdUnderlineSingle=1,			//A single line. default.
    wdUnderlineThick=6,				//A single thick line.
    wdUnderlineWavy=11,				//A single wavy line.
    wdUnderlineWavyDouble=43,		//A double wavy line.
    wdUnderlineWavyHeavy=27,		//A heavy wavy line.
    wdUnderlineWords=2				//Underline individual words only.
};

//前景色
enum WdColor{
    wdColorAqua = 13421619,//Aqua color.
    wdColorAutomatic = -16777216,//Automatic color. Default; usually black.
    wdColorBlack = 0,//Black color.
    wdColorBlue = 16711680,//Blue color.
    wdColorBlueGray = 10053222,//Blue-gray color.
    wdColorBrightGreen = 65280,//Bright green color.
    wdColorBrown = 13209,//Brown color.
    wdColorDarkBlue = 8388608,//Dark blue color.
    wdColorDarkGreen = 13056,//Dark green color.
    wdColorDarkRed = 128,//Dark red color.
    wdColorDarkTeal = 6697728,//Dark teal color.
    wdColorDarkYellow = 32896,//Dark yellow color.
    wdColorGold = 52479,//Gold color.
    wdColorGray05 = 15987699,//Shade 05 of gray color.
    wdColorGray10 = 15132390,//Shade 10 of gray color.
    wdColorGray125 = 14737632,//Shade 125 of gray color.
    wdColorGray15 = 14277081,//Shade 15 of gray color.
    wdColorGray20 = 13421772,//Shade 20 of gray color.
    wdColorGray25 = 12632256,//Shade 25 of gray color.
    wdColorGray30 = 11776947,//Shade 30 of gray color.
    wdColorGray35 = 10921638,//Shade 35 of gray color.
    wdColorGray375 = 10526880,//Shade 375 of gray color.
    wdColorGray40 = 10066329,//Shade 40 of gray color.
    wdColorGray45 = 9211020,//Shade 45 of gray color.
    wdColorGray50 = 8421504,//Shade 50 of gray color.
    wdColorGray55 = 7566195,//Shade 55 of gray color.
    wdColorGray60 = 6710886,//Shade 60 of gray color.
    wdColorGray625 = 6316128,//Shade 625 of gray color.
    wdColorGray65 = 5855577,//Shade 65 of gray color.
    wdColorGray70 = 5000268,//Shade 70 of gray color.
    wdColorGray75 = 4210752,//Shade 75 of gray color.
    wdColorGray80 = 3355443,//Shade 80 of gray color.
    wdColorGray85 = 2500134,//Shade 85 of gray color.
    wdColorGray875 = 2105376,//Shade 875 of gray color.
    wdColorGray90 = 1644825,//Shade 90 of gray color.
    wdColorGray95 = 789516,//Shade 95 of gray color.
    wdColorGreen = 32768,//Green color.
    wdColorIndigo = 10040115,//Indigo color.
    wdColorLavender = 16751052,//Lavender color.
    wdColorLightBlue = 16737843,//Light blue color.
    wdColorLightGreen = 13434828,//Light green color.
    wdColorLightOrange = 39423,//Light orange color.
    wdColorLightTurquoise = 16777164,//Light turquoise color.
    wdColorLightYellow = 10092543,//Light yellow color.
    wdColorLime = 52377,//Lime color.
    wdColorOliveGreen = 13107,//Olive green color.
    wdColorOrange = 26367,//Orange color.
    wdColorPaleBlue = 16764057,//Pale blue color.
    wdColorPink = 16711935,//Pink color.
    wdColorPlum = 6697881,//Plum color.
    wdColorRed = 255,//Red color.
    wdColorRose = 13408767,//Rose color.
    wdColorSeaGreen = 6723891,//Sea green color.
    wdColorSkyBlue = 16763904,//Sky blue color.
    wdColorTan = 10079487,//Tan color.
    wdColorTeal = 8421376,//Teal color.
    wdColorTurquoise = 16776960,//Turquoise color.
    wdColorViolet = 8388736,//Violet color.
    wdColorWhite = 16777215,//White color.
    wdColorYellow = 65535//Yellow color.
};

//表格边线
enum WdBorderType{
    wdBorderTop = 1,//A top border.
    wdBorderLeft,//A left border.
    wdBorderBottom,//A bottom border.
    wdBorderRight,//A right border.
    wdBorderHorizontal,//Horizontal borders.
    wdBorderVertical,//Vertical borders.
    wdBorderDiagonalDown,//A diagonal border starting in the top left corner.
    wdBorderDiagonalUp//A diagonal border starting in the bottom left corner.
};

//背景色
enum WdColorIndex{
    wdAuto = 0,//Automatic color. Default; usually black.
    wdBlack = 1,//Black color.
    wdBlue = 2,//Blue color.
    wdBrightGreen = 4,//Bright green color.
    wdByAuthor = -1,//Color defined by document author.
    wdDarkBlue = 9,//Dark blue color.
    wdDarkRed = 13,//Dark red color.
    wdDarkYellow = 14,//Dark yellow color.
    wdGray25 = 16,//Shade 25 of gray color.
    wdGray50 = 15,//Shade 50 of gray color.
    wdGreen = 11,//Green color.
    wdNoHighlight = 0,//Removes highlighting that has been applied.
    wdPink = 5,//Pink color.
    wdRed = 6,//Red color.
    wdTeal = 10,//Teal color.
    wdTurquoise = 3,//Turquoise color.
    wdViolet = 12,//Violet color.
    wdWhite = 8,//White color.
    wdYellow = 7,//Yellow color.
};

//页面视图
enum WdViewType{
    wdNormalView = 1,
    wdOutlineView = 2,
    wdPrintView = 3,
    wdPrintPreview = 4,
    wdMasterView = 5,
    wdWebView = 6,
    wdReadingView = 7
};

// 表格自动调整
enum WdAutoFitBehavior {
    wdAutoFitFixed = 0,
    wdAutoFitContent = 1,
    wdAutoFitWindow = 2
};

enum WdRowAlignment{
    wdAlignRowLeft = 0,
    wdAlignRowCenter = 1,
    wdAlignRowRight = 2
};

enum WdCellVerticalAlignment{
    wdCellAlignVerticalTop = 0,
    wdCellAlignVerticalCenter = 1,
    wdCellAlignVerticalBottom = 3
};

enum WdLineStyle{
    wdLineStyleDashDot = 5,
    wdLineStyleDashDotDot = 6,
    wdLineStyleDashDotStroked = 20,
    wdLineStyleDashLargeGap = 4,
    wdLineStyleDashSmallGap = 3,
    wdLineStyleDot = 2,
    wdLineStyleDouble = 7,
    wdLineStyleDoubleWavy = 19,
    wdLineStyleEmboss3D = 21,
    wdLineStyleEngrave3D = 22,
    wdLineStyleInset = 24,
    wdLineStyleNone = 0,
    wdLineStyleOutset = 23,
    wdLineStyleSingle = 1,
    wdLineStyleSingleWavy = 18,
    wdLineStyleThickThinLargeGap = 16,
    wdLineStyleThickThinMedGap = 13,
    wdLineStyleThickThinSmallGap = 10,
    wdLineStyleThinThickLargeGap = 15,
    wdLineStyleThinThickMedGap = 12,
    wdLineStyleThinThickSmallGap = 9,
    wdLineStyleThinThickThinLargeGap = 17,
    wdLineStyleThinThickThinMedGap = 14,
    wdLineStyleThinThickThinSmallGap = 11,
    wdLineStyleTriple = 8
};

enum WdHeaderFooterIndex{
    wdHeaderFooterEvenPages = 3,
    wdHeaderFooterFirstPage = 2,
    wdHeaderFooterPrimary = 1
};

}

class QWD_LIB_DECL QtWordHeaderFooter
{
public:
    QtWordHeaderFooter(){}
    virtual ~QtWordHeaderFooter(){}
public:
    //页眉文字
    QString sPageHeader;
    //页眉对齐方式
    Word::WdParagraphAlignment headerAlign;
    //页脚:前缀+页数+后缀
    QString sPageFooterPrefix;
    int nPageNo;
    QString sPageFooterSuffix;
    //页脚对齐方式
    Word::WdParagraphAlignment footerAlign;

};

class QWD_LIB_DECL QtWordTable
{
public:
    QtWordTable(QAxObject *word = 0, QAxObject *table = 0);
    virtual ~QtWordTable();
    QtWordTable& operator =(const QtWordTable &other);

    bool isValid();

    bool mergeCell(int startRow, int startCol, int endRow, int endCol);
    bool splitCell(int row, int col, int rows, int cols);
    bool splitCell(int startRow, int startCol, int endRow, int endCol, int rows, int cols);

    // color
    bool setTableTextColor(const QColor &color);
    bool setTableTextBackColor(Word::WdColorIndex colorIndex);
    bool setTableBackColor(Word::WdColor color);
    bool setTableBackColor(const QColor &color);

    bool setRowTextColor(int row, const QColor &color);
    bool setRowTextBackColor(int row, const QColor &color);
    bool setRowTextBackColor(int row, Word::WdColorIndex colorIndex);
    bool setRowBackColor(int row, Word::WdColor color);
    bool setRowBackColor(int row, const QColor &color);

    bool setColTextColor(int col, const QColor &color);
    bool setColTextBackColor(int col, const QColor &color);
    bool setColTextBackColor(int col, Word::WdColorIndex colorIndex);
    bool setColBackColor(int col, Word::WdColor color);
    bool setColBackColor(int col, const QColor &color);

    bool setCellTextColor(int row, int col,  const QColor &color);
    bool setCellTextBackColor(int row, int col, const QColor &color);
    bool setCellTextBackColor(int row, int col, Word::WdColorIndex colorIndex);

    bool setCellBackColor(int row, int col, Word::WdColorIndex colorIndex);
    bool setCellBackColor(int row, int col, Word::WdColor color);
    bool setCellBackColor(int row, int col, const QColor &color);

    // font
    bool setTableFont(const QFont &font);
    bool setTableFont(const QString &family, int pointSize = -1, bool bold = false, bool italic = false);
    bool setTableFontSize(int size);

    bool setRowFont(int row, const QFont &font);
    bool setRowFont(int row, const QString &family, int pointSize = -1, bool bold = false, bool italic = false);
    bool setRowFontBold(int row, bool bold);
    bool setRowFontItalic(int row, bool italic);
    bool setRowFontSize(int row, int size);

    bool setColFont(int col, const QFont &font);
    bool setColFont(int col, const QString &family, int pointSize = -1, bool bold = false, bool italic = false);
    bool setColFontBold(int col, bool bold);
    bool setColFontItalic(int col, bool italic);
    bool setColFontSize(int col, int size);

    bool setCellFont(int row, int col, const QFont &font);
    bool setCellFont(int row, int col, const QString &family, int pointSize = -1, bool bold = false, bool italic = false);
    bool setCellFontBold(int row, int col, bool bold);
    bool setCellFontItalic(int row, int col, bool italic);
    bool setCellFontSize(int row, int col, int size);

    // align
    bool setTableAlignment(Word::WdParagraphAlignment align);
    bool setRowAlignment(int row, Word::WdParagraphAlignment align);
    bool setColAlignment(int col, Word::WdParagraphAlignment align);
    bool setCellVerticalAlignment(int row, int col, Word::WdCellVerticalAlignment align);
    bool setCellParagraphAlignment(int row, int col, Word::WdParagraphAlignment align);

    // insert
    bool insertCellText(int row, int col, const QString &text);
    bool insertCellPicture(int row, int col, const QString &name);

    // size
    void setColumnWidth(int col, int width);
    void setRowHeight(int row, int height);

    // border
    void setInsideLineStyle(Word::WdLineStyle style);
    void setInsideLineWidth(int width);
    void setInsideColor(const QColor &color);
    void setInsideColorIndex(Word::WdColorIndex colorIndex);

    void setOutsideLineStyle(Word::WdLineStyle style);
    void setOutsideLineWidth(int width);
    void setOutsideColor(const QColor &color);
    void setOutsideColorIndex(Word::WdColorIndex colorIndex);

    // other
    void setAutoFitBehavior(Word::WdAutoFitBehavior behavior);

    //
    QString cellText(int row, int col);
    QVariant cellValue(int row, int col);

    void deleteColumn(int col);
    void deleteRow(int row);
    void addRow(int referRow, int count = 1);       // front
    void addColumn(int referColumn, int count = 1); // left
    void appendRow(int count = 1);      // rear
    void appendColumn(int count = 1);   // right

private:
    QAxObject* m_word;
    QAxObject* m_table;
};

class QWD_LIB_DECL QtWord : public QObject
{
    Q_OBJECT
public:
    explicit QtWord(QObject *parent = 0);
    QtWord(const QString &name, QObject *parent = 0);
    ~QtWord();

    bool open(bool visible = false);
    bool open(const QString &name, bool visible = false);
    bool close();
    bool isOpen();
    void save();
    void saveAs(const QString &name);
    QString version();

    // text
    void insertText(const QString &text);

    void setTextFont(const QFont &font);
    void setTextFont(const QString &family, int pointSize = -1, bool bold = false, bool italic = false);
    void setTextFontSize(int size);
    void setTextFontName(const QString &name);
    void setTextFontBold(bool bold);
    void setTextFontItalic(bool italic);

    void setTextColor(Word::WdColor color);
    void setTextColor(const QColor &color);
    void setTextBackColor(const QColor &color);
    void setTextBackColor(Word::WdColorIndex colorIndex);

    void setTextAlignment(Word::WdParagraphAlignment align);
    void setTextStyle(Word::WdBuiltinStyle style);
    void setTextUnderline(Word::WdUnderline underline);
    void setTextUnderlineColor(const QColor &color);

    // newline
    void insertNewline();

    // image (not done)
    void insertImage(const QString &name);
    void insertImage(QPixmap pixmap);

    // header footer (not done)
    void insertHeaderFooter(QtWordHeaderFooter headerfooter);

    // table
    QtWordTable insertTable(int rows, int cols);
    QtWordTable table(int index);
    int tableCount();

    // other
    void setPageOrientation(Word::WdOrientation orient);
    void setPageViewType(Word::WdViewType type);

    void moveForEnd();

    QString text();

private:
    QAxObject *m_word;
    QAxObject *m_documents;
    QAxObject *m_document;
    bool m_bOpened;
    QString m_fileName;
};

#endif // QTWORD_H
