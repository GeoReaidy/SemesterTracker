#include "appstyle.h"

#include <QApplication>
#include <QEvent>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QScreen>
#include <QSizePolicy>
#include <QTimer>

#include <algorithm>

namespace
{
class MessageBoxAutoSizer final : public QObject
{
public:
    using QObject::QObject;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        auto *box = qobject_cast<QMessageBox *>(watched);
        if (!box)
            return QObject::eventFilter(watched, event);

        if (event->type() == QEvent::Show ||
            event->type() == QEvent::LayoutRequest ||
            event->type() == QEvent::FontChange)
        {
            QTimer::singleShot(0, box, [box]() { resizeMessageBox(box); });
        }

        return QObject::eventFilter(watched, event);
    }

private:
    static int naturalTextWidth(const QLabel *label)
    {
        if (!label)
            return 0;

        const QFontMetrics metrics(label->font());
        int widestLine = 0;
        const QStringList lines = label->text().split('\n');
        for (const QString &line : lines)
            widestLine = std::max(widestLine, metrics.horizontalAdvance(line));

        return widestLine + 28;
    }

    static void prepareLabel(QLabel *label, int width)
    {
        if (!label)
            return;

        label->setWordWrap(true);
        label->setMinimumWidth(width);
        label->setMaximumWidth(width);
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        label->updateGeometry();
    }

    static void resizeMessageBox(QMessageBox *box)
    {
        if (!box || !box->isVisible())
            return;

        QLabel *mainLabel = box->findChild<QLabel *>(QStringLiteral("qt_msgbox_label"));
        QLabel *infoLabel = box->findChild<QLabel *>(QStringLiteral("qt_msgbox_informativelabel"));

        QScreen *screen = box->screen();
        if (!screen)
            screen = QGuiApplication::primaryScreen();

        const int screenWidth = screen ? screen->availableGeometry().width() : 1280;
        const int maximumTextWidth = std::max(320, std::min(720, screenWidth - 220));
        const int minimumTextWidth = 240;

        const int requestedWidth = std::max(
            naturalTextWidth(mainLabel),
            naturalTextWidth(infoLabel));

        const int textWidth = std::clamp(
            requestedWidth,
            minimumTextWidth,
            maximumTextWidth);

        prepareLabel(mainLabel, textWidth);
        prepareLabel(infoLabel, textWidth);

        if (QLayout *layout = box->layout())
        {
            layout->invalidate();
            layout->activate();
        }

        box->setMinimumSize(QSize(0, 0));
        box->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        box->adjustSize();
    }
};

void installMessageBoxAutoSizing()
{
    static MessageBoxAutoSizer *sizer = nullptr;

    if (!qApp || sizer)
        return;

    sizer = new MessageBoxAutoSizer(qApp);
    qApp->installEventFilter(sizer);
}
}

QString AppStyle::globalStyleSheet()
{
    installMessageBoxAutoSizing();

    return QStringLiteral(R"(
        QWidget {
            color: #1f2937;
            font-family: "Segoe UI";
            font-size: 13px;
        }

        QDialog {
            background-color: #f8fafc;
        }

        QWidget[page="true"] {
            background-color: #f5f7fb;
        }

        QFrame[card="true"] {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 12px;
        }

        QWidget[rowCard="true"] {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 9px;
        }

        QLabel[role="pageTitle"] {
            color: #111827;
            font-size: 25px;
            font-weight: 700;
        }

        QLabel[role="pageSubtitle"],
        QLabel[role="dialogSubtitle"] {
            color: #64748b;
            font-size: 13px;
        }

        QLabel[role="dialogTitle"] {
            color: #0f172a;
            font-size: 22px;
            font-weight: 700;
        }

        QLabel[role="sectionTitle"] {
            color: #111827;
            font-size: 16px;
            font-weight: 700;
        }

        QLabel[role="fieldLabel"] {
            color: #334155;
            font-size: 13px;
            font-weight: 600;
        }

        QLabel[role="rowTitle"] {
            color: #111827;
            font-size: 15px;
            font-weight: 600;
        }

        QLabel[role="emptyState"] {
            color: #94a3b8;
            font-size: 14px;
        }

        QLabel[role="summaryPill"] {
            color: #475569;
            background-color: #f8fafc;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 4px 9px;
            font-size: 12px;
        }

        QLabel[role="notice"] {
            color: #92400e;
            background-color: #fffbeb;
            border: 1px solid #fde68a;
            border-radius: 7px;
            padding: 8px;
        }

        QLabel[role="validation"] {
            color: #b91c1c;
            background-color: #fef2f2;
            border: 1px solid #fecaca;
            border-radius: 7px;
            padding: 8px;
        }

        QFrame[role="infoPanel"] {
            background-color: #eff6ff;
            border: 1px solid #bfdbfe;
            border-radius: 9px;
        }

        QListWidget[cardList="true"] {
            color: #1f2937;
            background-color: transparent;
            border: none;
            border-radius: 0;
            outline: none;
        }

        QListWidget[cardList="true"]::item {
            background-color: transparent;
            border: none;
            margin: 4px 0;
            padding: 0;
        }

        QListWidget[cardList="true"]::item:hover,
        QListWidget[cardList="true"]::item:selected {
            color: #1f2937;
            background-color: transparent;
        }

        QToolTip {
            color: #ffffff;
            background-color: #111827;
            border: 1px solid #374151;
            border-radius: 5px;
            padding: 5px 8px;
        }

        QLineEdit,
        QTextEdit,
        QPlainTextEdit,
        QSpinBox,
        QDoubleSpinBox,
        QDateEdit,
        QTimeEdit,
        QDateTimeEdit,
        QComboBox {
            min-height: 34px;
            color: #1f2937;
            background-color: #ffffff;
            border: 1px solid #cbd5e1;
            border-radius: 8px;
            padding: 0 10px;
            selection-background-color: #2563eb;
            selection-color: #ffffff;
        }

        QTextEdit,
        QPlainTextEdit {
            padding: 8px 10px;
        }

        /* Clean, compact stepper buttons for numeric/date/time fields. */
        QSpinBox,
        QDoubleSpinBox,
        QDateEdit,
        QTimeEdit,
        QDateTimeEdit {
            padding-right: 30px;
        }

        QSpinBox::up-button,
        QDoubleSpinBox::up-button,
        QDateEdit::up-button,
        QTimeEdit::up-button,
        QDateTimeEdit::up-button {
            subcontrol-origin: border;
            subcontrol-position: top right;
            width: 26px;
            background-color: #f8fafc;
            border: none;
            border-left: 1px solid #cbd5e1;
            border-bottom: 1px solid #e2e8f0;
            border-top-right-radius: 7px;
        }

        QSpinBox::down-button,
        QDoubleSpinBox::down-button,
        QDateEdit::down-button,
        QTimeEdit::down-button,
        QDateTimeEdit::down-button {
            subcontrol-origin: border;
            subcontrol-position: bottom right;
            width: 26px;
            background-color: #f8fafc;
            border: none;
            border-left: 1px solid #cbd5e1;
            border-top: 1px solid #e2e8f0;
            border-bottom-right-radius: 7px;
        }

        QSpinBox::up-button:hover,
        QDoubleSpinBox::up-button:hover,
        QDateEdit::up-button:hover,
        QTimeEdit::up-button:hover,
        QDateTimeEdit::up-button:hover,
        QSpinBox::down-button:hover,
        QDoubleSpinBox::down-button:hover,
        QDateEdit::down-button:hover,
        QTimeEdit::down-button:hover,
        QDateTimeEdit::down-button:hover {
            background-color: #e2e8f0;
        }

        QSpinBox::up-button:pressed,
        QDoubleSpinBox::up-button:pressed,
        QDateEdit::up-button:pressed,
        QTimeEdit::up-button:pressed,
        QDateTimeEdit::up-button:pressed,
        QSpinBox::down-button:pressed,
        QDoubleSpinBox::down-button:pressed,
        QDateEdit::down-button:pressed,
        QTimeEdit::down-button:pressed,
        QDateTimeEdit::down-button:pressed {
            background-color: #cbd5e1;
        }

        QSpinBox::up-arrow,
        QDoubleSpinBox::up-arrow,
        QDateEdit::up-arrow,
        QTimeEdit::up-arrow,
        QDateTimeEdit::up-arrow {
            image: url(:/icons/chevron-up-black.svg);
            width: 11px;
            height: 11px;
        }

        QSpinBox::down-arrow,
        QDoubleSpinBox::down-arrow,
        QDateEdit::down-arrow,
        QTimeEdit::down-arrow,
        QDateTimeEdit::down-arrow {
            image: url(:/icons/chevron-down-black.svg);
            width: 11px;
            height: 11px;
        }

        QSpinBox::up-button:disabled,
        QDoubleSpinBox::up-button:disabled,
        QDateEdit::up-button:disabled,
        QTimeEdit::up-button:disabled,
        QDateTimeEdit::up-button:disabled,
        QSpinBox::down-button:disabled,
        QDoubleSpinBox::down-button:disabled,
        QDateEdit::down-button:disabled,
        QTimeEdit::down-button:disabled,
        QDateTimeEdit::down-button:disabled {
            background-color: #f1f5f9;
            border-color: #e2e8f0;
        }

        QLineEdit:hover,
        QTextEdit:hover,
        QPlainTextEdit:hover,
        QSpinBox:hover,
        QDoubleSpinBox:hover,
        QDateEdit:hover,
        QTimeEdit:hover,
        QDateTimeEdit:hover,
        QComboBox:hover {
            border-color: #94a3b8;
        }

        QLineEdit:focus,
        QTextEdit:focus,
        QPlainTextEdit:focus,
        QSpinBox:focus,
        QDoubleSpinBox:focus,
        QDateEdit:focus,
        QTimeEdit:focus,
        QDateTimeEdit:focus,
        QComboBox:focus {
            border-color: #2563eb;
        }

        QLineEdit:disabled,
        QTextEdit:disabled,
        QPlainTextEdit:disabled,
        QSpinBox:disabled,
        QDoubleSpinBox:disabled,
        QDateEdit:disabled,
        QTimeEdit:disabled,
        QDateTimeEdit:disabled,
        QComboBox:disabled {
            color: #94a3b8;
            background-color: #f1f5f9;
            border-color: #e2e8f0;
        }

        /* Calendar-popup date fields use one full-height drop-down button.
           Keep this after the generic spin-box rules so QDateEdit does not
           inherit the stacked numeric stepper appearance. */
        QDateEdit {
            padding-right: 36px;
        }

        QDateEdit::drop-down {
            subcontrol-origin: border;
            subcontrol-position: top right;
            width: 32px;
            background-color: #f8fafc;
            border: none;
            border-left: 1px solid #cbd5e1;
            border-top-right-radius: 7px;
            border-bottom-right-radius: 7px;
        }

        QDateEdit::drop-down:hover {
            background-color: #e2e8f0;
        }

        QDateEdit::drop-down:pressed {
            background-color: #cbd5e1;
        }

        QDateEdit::drop-down:disabled {
            background-color: #f1f5f9;
            border-left-color: #e2e8f0;
        }

        QDateEdit::down-arrow {
            image: url(:/icons/chevron-down-black.svg);
            width: 12px;
            height: 12px;
        }

        /* QCalendarWidget popup */
        QCalendarWidget {
            color: #1f2937;
            background-color: #ffffff;
            border: 1px solid #cbd5e1;
            border-radius: 10px;
        }

        QCalendarWidget QWidget#qt_calendar_navigationbar {
            min-height: 44px;
            background-color: #ffffff;
            border: none;
            border-bottom: 1px solid #e2e8f0;
        }

        QCalendarWidget QToolButton {
            min-width: 30px;
            max-width: none;
            min-height: 30px;
            max-height: 30px;
            color: #1f2937;
            background-color: transparent;
            border: none;
            border-radius: 7px;
            padding: 0 8px;
            margin: 6px 2px;
            font-size: 13px;
            font-weight: 600;
        }

        QCalendarWidget QToolButton:hover {
            background-color: #eff6ff;
            color: #1d4ed8;
        }

        QCalendarWidget QToolButton:pressed,
        QCalendarWidget QToolButton:checked {
            background-color: #dbeafe;
            color: #1e40af;
        }

        QCalendarWidget QToolButton#qt_calendar_prevmonth,
        QCalendarWidget QToolButton#qt_calendar_nextmonth {
            min-width: 30px;
            max-width: 30px;
            padding: 0;
            qproperty-iconSize: 14px 14px;
        }

        QCalendarWidget QToolButton#qt_calendar_prevmonth {
            qproperty-icon: url(:/icons/chevron-left-black.svg);
        }

        QCalendarWidget QToolButton#qt_calendar_nextmonth {
            qproperty-icon: url(:/icons/chevron-right-black.svg);
        }

        QCalendarWidget QToolButton::menu-indicator {
            image: url(:/icons/chevron-down-black.svg);
            width: 10px;
            height: 10px;
            subcontrol-origin: padding;
            subcontrol-position: center right;
            right: 5px;
        }

        QCalendarWidget QSpinBox {
            min-width: 74px;
            min-height: 30px;
            max-height: 30px;
            margin: 6px 4px;
            padding-left: 8px;
            padding-right: 26px;
            background-color: #ffffff;
            border: 1px solid #cbd5e1;
            border-radius: 7px;
        }

        QCalendarWidget QAbstractItemView {
            color: #334155;
            background-color: #ffffff;
            border: none;
            outline: none;
            selection-background-color: #2563eb;
            selection-color: #ffffff;
            alternate-background-color: #ffffff;
        }

        QCalendarWidget QAbstractItemView:enabled {
            color: #334155;
            background-color: #ffffff;
            selection-background-color: #2563eb;
            selection-color: #ffffff;
        }

        QCalendarWidget QAbstractItemView:disabled {
            color: #cbd5e1;
        }

        QCalendarWidget QMenu {
            min-width: 120px;
        }

        QComboBox {
            padding-right: 30px;
        }

        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 28px;
            border-left: 1px solid #cbd5e1;
        }

        QComboBox::down-arrow {
            image: url(:/icons/chevron-down-black.svg);
            width: 13px;
            height: 13px;
        }

        QToolButton::menu-indicator {
            image: url(:/icons/chevron-down-black.svg);
            width: 12px;
            height: 12px;
            subcontrol-origin: padding;
            subcontrol-position: center right;
            right: 8px;
        }

        QComboBox QAbstractItemView {
            color: #1f2937;
            background-color: #ffffff;
            border: 1px solid #cbd5e1;
            border-radius: 7px;
            outline: none;
            padding: 4px;
            selection-background-color: #2563eb;
            selection-color: #ffffff;
        }

        QComboBox QAbstractItemView::item {
            min-height: 30px;
            color: #1f2937;
            background-color: #ffffff;
            padding: 4px 9px;
        }

        QComboBox QAbstractItemView::item:hover {
            color: #1f2937;
            background-color: #e2e8f0;
        }

        QComboBox QAbstractItemView::item:selected {
            color: #ffffff;
            background-color: #2563eb;
        }

        QMenu {
            color: #1f2937;
            background-color: #ffffff;
            border: 1px solid #cbd5e1;
            border-radius: 8px;
            padding: 5px;
            font-size: 13px;
            font-weight: 600;
        }

        QMenu::item {
            min-width: 145px;
            color: #1f2937;
            background-color: transparent;
            border-radius: 6px;
            padding: 9px 14px;
            margin: 2px;
        }

        QMenu::item:selected {
            color: #ffffff;
            background-color: #2563eb;
        }

        QMenu::item:disabled {
            color: #94a3b8;
            background-color: transparent;
        }

        QMenu::separator {
            height: 1px;
            background-color: #e2e8f0;
            margin: 5px 8px;
        }

        QMenu::indicator {
            width: 14px;
            height: 14px;
        }

        QPushButton {
            min-height: 34px;
            color: #334155;
            background-color: #ffffff;
            border: 1px solid #cbd5e1;
            border-radius: 8px;
            padding: 0 14px;
            font-weight: 600;
        }

        QPushButton:hover {
            background-color: #f8fafc;
            border-color: #94a3b8;
        }

        QPushButton:pressed {
            background-color: #f1f5f9;
        }

        QPushButton:disabled {
            color: #94a3b8;
            background-color: #f1f5f9;
            border-color: #e2e8f0;
        }

        QPushButton[role="primary"] {
            color: #ffffff;
            background-color: #2563eb;
            border-color: #2563eb;
        }

        QPushButton[role="primary"]:hover {
            background-color: #1d4ed8;
            border-color: #1d4ed8;
        }

        QPushButton[role="primary"]:pressed {
            background-color: #1e40af;
            border-color: #1e40af;
        }

        QPushButton[role="danger"] {
            color: #ffffff;
            background-color: #dc2626;
            border-color: #dc2626;
        }

        QPushButton[role="danger"]:hover {
            background-color: #b91c1c;
            border-color: #b91c1c;
        }

        QPushButton[role="secondary"] {
            color: #334155;
            background-color: #ffffff;
            border-color: #cbd5e1;
        }

        QDialogButtonBox QPushButton {
            min-width: 96px;
            min-height: 38px;
            padding: 0 16px;
        }

        QToolButton {
            min-height: 30px;
            color: #334155;
            background-color: #ffffff;
            border: 1px solid #cbd5e1;
            border-radius: 8px;
            padding: 3px 9px;
            font-weight: 600;
        }

        QToolButton:hover {
            background-color: #f8fafc;
            border-color: #94a3b8;
        }

        QToolButton:pressed {
            background-color: #f1f5f9;
        }

        QToolButton[role="editAction"],
        QToolButton[role="deleteAction"] {
            min-width: 32px;
            max-width: 32px;
            min-height: 32px;
            max-height: 32px;
            border: none;
            border-radius: 7px;
            padding: 0;
        }

        QToolButton[role="editAction"] {
            color: #2563eb;
            background-color: #eff6ff;
            font-size: 16px;
        }

        QToolButton[role="editAction"]:hover {
            background-color: #dbeafe;
        }

        QToolButton[role="deleteAction"] {
            color: #dc2626;
            background-color: #fef2f2;
            font-size: 15px;
        }

        QToolButton[role="deleteAction"]:hover {
            background-color: #fee2e2;
        }

        QToolButton[status] {
            min-width: 108px;
            min-height: 32px;
            max-height: 32px;
            border-radius: 8px;
            padding: 5px 24px 5px 10px;
            font-size: 12px;
            font-weight: 600;
        }

        QToolButton[status]::menu-indicator {
            subcontrol-origin: padding;
            subcontrol-position: center right;
            right: 7px;
        }

        QToolButton[status="planned"] {
            color: #475569;
            background-color: #f1f5f9;
            border: 1px solid #cbd5e1;
        }

        QToolButton[status="planned"]:hover {
            background-color: #e2e8f0;
        }

        QToolButton[status="active"] {
            color: #166534;
            background-color: #dcfce7;
            border: 1px solid #bbf7d0;
        }

        QToolButton[status="active"]:hover {
            background-color: #bbf7d0;
        }

        QToolButton[status="completed"] {
            color: #1d4ed8;
            background-color: #dbeafe;
            border: 1px solid #bfdbfe;
        }

        QToolButton[status="completed"]:hover {
            background-color: #bfdbfe;
        }

        QToolButton[status="completed"]:disabled {
            color: #1d4ed8;
            background-color: #dbeafe;
            border: 1px solid #bfdbfe;
        }

        QCheckBox,
        QRadioButton {
            color: #334155;
            spacing: 8px;
        }

        QCheckBox:disabled,
        QRadioButton:disabled {
            color: #94a3b8;
        }

        QListWidget,
        QTreeWidget,
        QTableWidget,
        QListView,
        QTreeView,
        QTableView {
            color: #1f2937;
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            outline: none;
            selection-background-color: #dbeafe;
            selection-color: #1f2937;
        }

        QHeaderView::section {
            color: #475569;
            background-color: #f8fafc;
            border: none;
            border-bottom: 1px solid #e2e8f0;
            padding: 7px;
            font-weight: 600;
        }

        QGroupBox {
            color: #334155;
            border: 1px solid #e2e8f0;
            border-radius: 10px;
            margin-top: 12px;
            padding-top: 10px;
            font-weight: 600;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 10px;
            padding: 0 5px;
            background-color: #ffffff;
        }

        QScrollBar:vertical {
            width: 10px;
            margin: 2px;
            background-color: transparent;
        }

        QScrollBar::handle:vertical {
            min-height: 28px;
            background-color: #cbd5e1;
            border-radius: 5px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: #94a3b8;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical,
        QScrollBar::add-page:vertical,
        QScrollBar::sub-page:vertical {
            height: 0;
            background: transparent;
        }

        QScrollBar:horizontal {
            height: 10px;
            margin: 2px;
            background-color: transparent;
        }

        QScrollBar::handle:horizontal {
            min-width: 28px;
            background-color: #cbd5e1;
            border-radius: 5px;
        }

        QScrollBar::handle:horizontal:hover {
            background-color: #94a3b8;
        }

        QScrollBar::add-line:horizontal,
        QScrollBar::sub-line:horizontal,
        QScrollBar::add-page:horizontal,
        QScrollBar::sub-page:horizontal {
            width: 0;
            background: transparent;
        }

        QMessageBox {
            background-color: #f8fafc;
        }

        /* Width is calculated per message box by MessageBoxAutoSizer. */
        QMessageBox QLabel#qt_msgbox_label,
        QMessageBox QLabel#qt_msgbox_informativelabel {
            color: #1f2937;
            padding: 8px 6px;
            font-size: 13px;
        }

        /* Reserve enough space for the information/warning icon */
        QMessageBox QLabel#qt_msgboxex_icon_label {
            min-width: 42px;
            max-width: 42px;
            padding: 0 8px 0 0;
        }

        /* Compact but readable buttons */
        QMessageBox QPushButton {
            min-width: 92px;
            min-height: 34px;
            max-height: 34px;
            padding: 0 16px;
            margin: 0 2px;
            border-radius: 7px;
        }

        QMessageBox QPushButton:hover {
            background-color: #f1f5f9;
            border-color: #94a3b8;
        }

        QMessageBox QPushButton:pressed {
            background-color: #e2e8f0;
        }
    )");
}
