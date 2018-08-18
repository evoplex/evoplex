#ifndef ATTRCOLORSELECTOR_H
#define ATTRCOLORSELECTOR_H

#include <QWidget>

#include "core/include/attributerange.h"
#include "colormap.h"

class Ui_AttrColorSelector;

namespace evoplex {
class AttrColorSelector : public QWidget
{
    Q_OBJECT

public:
    explicit AttrColorSelector(QWidget *parent = nullptr);
    ~AttrColorSelector();

    void init(ColorMapMgr* cmapMgr, AttributesScope scope);
    inline ColorMap* cmap() const;

signals:
    void cmapUpdated(ColorMap* cmap);

private slots:
    void updateCMap();
    void slotCMapName(const QString& name);

private:
    Ui_AttrColorSelector* m_ui;
    ColorMapMgr* m_cmapMgr;
    AttributesScope m_attrScope;
    ColorMap* m_cmap;

    void blockAllSignals(bool b);
};

inline ColorMap* AttrColorSelector::cmap() const
{ return m_cmap; }

} // evoplex
#endif // ATTRCOLORSELECTOR_H
