#ifndef CEBITOR_H
#define CEBITOR_H

#include "QsciLexerGlsl.h"

#include <Qsci/qsciscintilla.h>
#include <QLineEdit>

//------------------------------------------------------------------------------------------------------------------------
/// @file Cebitor.h
/// @brief CEB text editor, implementation of QScintilla
/// @author Phil Rouse
/// @version 1.0
/// @date 03/03/16
//------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------
/// @class Cebitor
/// @brief CEB text editor class derived from QsciScintilla
//------------------------------------------------------------------------------------------------------------------------
class Cebitor : public QsciScintilla
{
  Q_OBJECT;

public:
  //----------------------------------------------------------------------------------------------------------------------
  /// \brief enum for supported style types
  //----------------------------------------------------------------------------------------------------------------------
  enum MarkerType
  {
    ERROR,
    WARNING,
    FILESTART
  };
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief Cebitor constructor, initialises default values
  //------------------------------------------------------------------------------------------------------------------------
  Cebitor(QWidget *_parent);

  inline void setSearchWidget(QWidget *_searchWidget) {m_searchWidget = _searchWidget;}

  inline void setSearchLineEdit(QLineEdit *_searchLineEdit) {m_searchLineEdit = _searchLineEdit;}

public slots:
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief Removes all error and warning line markers
  //------------------------------------------------------------------------------------------------------------------------
  void clearErrors();
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief Search for next occurance
  //------------------------------------------------------------------------------------------------------------------------
  void searchNext();
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief Search for previous occurance
  //------------------------------------------------------------------------------------------------------------------------
  void searchPrev();
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief sets indicator for each search result
  //------------------------------------------------------------------------------------------------------------------------
  void highlightAllSearch();

protected:
  QWidget *m_searchWidget;

  QLineEdit *m_searchLineEdit;

  std::vector<int> m_fileMarkers;

  int m_searchIndicator;
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief adds closing character for braces and quotes
  /// @param [in] _close closing character
  /// @returns true if closing character is added
  //------------------------------------------------------------------------------------------------------------------------
  bool autoClose(const QString _close);
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief removes duplicates of closing braces and quotes
  /// @param [in] _close closing character
  /// @returns true if duplicate character is replaced
  //------------------------------------------------------------------------------------------------------------------------
  bool closing(const QString _close);
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief adds indentation after newline following a "{"
  //------------------------------------------------------------------------------------------------------------------------
  void braceIndent();

protected slots:
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief comment out all selected lines
  //------------------------------------------------------------------------------------------------------------------------
  void comment();
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief toggle search widget
  //------------------------------------------------------------------------------------------------------------------------
  void toggleSearchBox();
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief called when a character is added
  //------------------------------------------------------------------------------------------------------------------------
  void charAdded(int _c);
  //------------------------------------------------------------------------------------------------------------------------
  /// @brief reset the highlighting colour
  //------------------------------------------------------------------------------------------------------------------------
  void resetHighlightColour();
};

#endif
