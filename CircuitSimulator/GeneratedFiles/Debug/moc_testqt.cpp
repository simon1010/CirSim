/****************************************************************************
** Meta object code from reading C++ file 'testqt.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Local/Include/testqt.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'testqt.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TestQt_t {
    QByteArrayData data[14];
    char stringdata0[210];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TestQt_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TestQt_t qt_meta_stringdata_TestQt = {
    {
QT_MOC_LITERAL(0, 0, 6), // "TestQt"
QT_MOC_LITERAL(1, 7, 6), // "update"
QT_MOC_LITERAL(2, 14, 0), // ""
QT_MOC_LITERAL(3, 15, 7), // "pushBtn"
QT_MOC_LITERAL(4, 23, 15), // "ShowContextMenu"
QT_MOC_LITERAL(5, 39, 3), // "pos"
QT_MOC_LITERAL(6, 43, 19), // "ms_xSelectRectangle"
QT_MOC_LITERAL(7, 63, 18), // "ms_xSelectDCSource"
QT_MOC_LITERAL(8, 82, 14), // "ms_xSelectWire"
QT_MOC_LITERAL(9, 97, 18), // "ms_xSelectResistor"
QT_MOC_LITERAL(10, 116, 16), // "ms_xSelectGround"
QT_MOC_LITERAL(11, 133, 35), // "ms_xSelectProgrammableVoltage..."
QT_MOC_LITERAL(12, 169, 19), // "ms_xSelectCapacitor"
QT_MOC_LITERAL(13, 189, 20) // "ms_xSelectMicrophone"

    },
    "TestQt\0update\0\0pushBtn\0ShowContextMenu\0"
    "pos\0ms_xSelectRectangle\0ms_xSelectDCSource\0"
    "ms_xSelectWire\0ms_xSelectResistor\0"
    "ms_xSelectGround\0ms_xSelectProgrammableVoltageSource\0"
    "ms_xSelectCapacitor\0ms_xSelectMicrophone"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TestQt[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x0a /* Public */,
       3,    0,   70,    2, 0x08 /* Private */,
       4,    1,   71,    2, 0x08 /* Private */,
       6,    0,   74,    2, 0x08 /* Private */,
       7,    0,   75,    2, 0x08 /* Private */,
       8,    0,   76,    2, 0x08 /* Private */,
       9,    0,   77,    2, 0x08 /* Private */,
      10,    0,   78,    2, 0x08 /* Private */,
      11,    0,   79,    2, 0x08 /* Private */,
      12,    0,   80,    2, 0x08 /* Private */,
      13,    0,   81,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TestQt::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TestQt *_t = static_cast<TestQt *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->update(); break;
        case 1: _t->pushBtn(); break;
        case 2: _t->ShowContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 3: _t->ms_xSelectRectangle(); break;
        case 4: _t->ms_xSelectDCSource(); break;
        case 5: _t->ms_xSelectWire(); break;
        case 6: _t->ms_xSelectResistor(); break;
        case 7: _t->ms_xSelectGround(); break;
        case 8: _t->ms_xSelectProgrammableVoltageSource(); break;
        case 9: _t->ms_xSelectCapacitor(); break;
        case 10: _t->ms_xSelectMicrophone(); break;
        default: ;
        }
    }
}

const QMetaObject TestQt::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_TestQt.data,
      qt_meta_data_TestQt,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TestQt::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TestQt::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TestQt.stringdata0))
        return static_cast<void*>(const_cast< TestQt*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int TestQt::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
