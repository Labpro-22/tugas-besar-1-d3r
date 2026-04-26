#pragma once
#include "../include/core/Tile.hpp"

#include "raylib.h"

class Tile;
class Street;
class Railroad;
class Utility;
class PBM;
class PPH;
class CardTile;
class Festival;
class Go;
class Prison;
class Trap;
class FreeParking;

class PopUpField {
private:
    std::string label_;
    std::string value_;

public:
    PopUpField() = default;
    PopUpField(std::string label, std::string value)
        : label_(std::move(label)), value_(std::move(value)) {}

    // Getters
    const std::string &getLabel() const { return label_; }
    const std::string &getValue() const { return value_; }

    // Setters
    void setLabel(std::string label) { label_ = std::move(label); }
    void setValue(std::string value) { value_ = std::move(value); }
};

class PopUpData {
private:
    std::string title_;
    std::string subtitle_;
    std::vector<PopUpField> fields_;
    std::string description_;

public:
    PopUpData() = default;
    PopUpData(std::string title, std::string subtitle,
              std::vector<PopUpField> fields, std::string description)
        : title_(std::move(title)), subtitle_(std::move(subtitle)),
          fields_(std::move(fields)), description_(std::move(description)) {}

    // Getters
    const std::string &getTitle() const { return title_; }
    const std::string &getSubtitle() const { return subtitle_; }
    const std::vector<PopUpField> &getFields() const { return fields_; }
    const std::string &getDescription() const { return description_; }

    // Setters
    void setTitle(std::string title) { title_ = std::move(title); }
    void setSubtitle(std::string subtitle) { subtitle_ = std::move(subtitle); }
    void setDescription(std::string description) { description_ = std::move(description); }

    // Fields — manipulasi koleksi
    void setFields(std::vector<PopUpField> fields) { fields_ = std::move(fields); }
    void addField(PopUpField field) { fields_.emplace_back(std::move(field)); }
    void clearFields() { fields_.clear(); }
    bool hasFields() const { return !fields_.empty(); }
    bool hasDescription() const { return !description_.empty(); }
};

class UIComponent {
protected:
    bool visible = false;

public:
    virtual ~UIComponent() = default;
    virtual void render() = 0;
    virtual void handleInput() = 0;
    void setVisible(bool status) { visible = status; }
    bool isVisible() const { return visible; }
};

class TilePopUpCall {
public:
    virtual ~TilePopUpCall() = default;
    // Properti
    virtual void renderPopUp(Street *s) = 0;
    virtual void renderPopUp(Railroad *r) = 0;
    virtual void renderPopUp(Utility *u) = 0;

    // Taxes
    virtual void renderPopUp(PBM *pbm) = 0;
    virtual void renderPopUp(PPH *pph) = 0;

    // Special Tiles
    virtual void renderPopUp(CardTile *ct) = 0;
    virtual void renderPopUp(Festival *f) = 0;
    virtual void renderPopUp(Go *g) = 0;
    virtual void renderPopUp(Prison *p) = 0;
    virtual void renderPopUp(Trap *tr) = 0;
    virtual void renderPopUp(FreeParking *fp) = 0;
};

class TilePopup : public UIComponent, public TilePopUpCall {
private:
    static constexpr int POPUP_W = 360;
    static constexpr int POPUP_H = 480;
    static constexpr int PADDING = 20;
    static constexpr int HEADER_H = 60;
    static constexpr int BTN_W = 110;
    static constexpr int BTN_H = 38;

    // font size
    static constexpr float FS_TITLE = 22.0f;
    static constexpr float FS_SUBTITLE = 14.0f;
    static constexpr float FS_FIELD = 16.0f;
    static constexpr float FS_BODY = 15.0f;
    static constexpr float SPACING = 1.0f;

    Rectangle cardRect_;
    Font font_;
    PopUpData data_;
    Color bgColor;
    Rectangle btnRect_;
    void drawPopUp();
    void updateButtonRect();
    void drawFields(int x, int &cursorY, int maxW);
    void drawParagraph(int x, int &cursorY, int maxW, int bottomLimit);

    // Text helpers
    std::string truncateToFit(const std::string &text,
                              float fontSize, int maxWidth) const;

    // Wrap teks jadi vector baris, tidak overflow maxWidth
    std::vector<std::string> wrapText(const std::string &text,
                                      float fontSize, int maxWidth) const;

    // Ukur teks dengan font_ dan fontSize
    Vector2 measure(const std::string &text, float fontSize) const;

public:
    TilePopup();
    void render() override;
    void handleInput() override;
    void renderPopUp(Street *s) override;
    void renderPopUp(Railroad *r) override;
    void renderPopUp(Utility *u) override;

    // Taxes
    void renderPopUp(PBM *pbm) override;
    void renderPopUp(PPH *pph) override;

    // Special Tiles
    void renderPopUp(CardTile *ct) override;
    void renderPopUp(Festival *f) override;
    void renderPopUp(Go *g) override;
    void renderPopUp(Prison *p) override;
    void renderPopUp(Trap *tr) override;
    void renderPopUp(FreeParking *fp) override;

    void setFont(Font font) { font_ = font; }

};
