#ifndef MATERIAL_H
#define MATERIAL_H

#include <QGLViewer/vec.h>
#include <qimage.h>
#include "color.h"

class Material
{
public :
	Material();

	enum TextureMode { MODULATE, BLEND, REPLACE };

	Color diffuseColor() const { return diffuseColor_; }
	Color diffuseColor(float u, float v) const;
	Color specularColor() const { return specularColor_; }
	Color reflectiveColor() const { return reflectiveColor_; }
	float specularCoefficient() const { return specularCoefficient_; }
	float textureScaleU() const { return textureScaleU_; }
	float textureScaleV() const { return textureScaleV_; }
	float refractionIndex() const { return refractionIndex_; }
	TextureMode textureMode() const { return textureMode_; }
	const QImage& texture() const { return texture_; }
	const QImage& bumpMap() const { return bump_map_; }
	float bumpMapScaleU() const { return bumpMapScaleU_; }
	float bumpMapScaleV() const { return bumpMapScaleV_; }
	Color perturbation(float u, float v) const;

	void setDiffuseColor(const Color& dc) { diffuseColor_ = dc; }
	void setSpecularColor(const Color& specularColor) { specularColor_ = specularColor; }
	void setReflectiveColor(const Color& reflectiveColor) { reflectiveColor_ = reflectiveColor; }
	void setSpecularCoefficient(float specularCoefficient) { specularCoefficient_ = specularCoefficient; }

	void setTexture(const QImage& texture) { texture_ = texture; }
	void loadTextureFromFile(const QString& fileName);
	void setTextureScale(float textureScale) { setTextureScaleU(textureScale); setTextureScaleV(textureScale); }
	void setTextureScaleU(float textureScaleU) { if (fabs(textureScaleU) > 1e-3) textureScaleU_ = textureScaleU; }
	void setTextureScaleV(float textureScaleV) { if (fabs(textureScaleV) > 1e-3) textureScaleV_ = textureScaleV; }
	void setTextureMode(TextureMode textureMode) { textureMode_ = textureMode; }

	void setBumpMap(const QImage& bump_map) { bump_map_ = bump_map; }
	void loadBumpMapFromFile(const QString& fileName);
	void setBumpMapScale(float bumpMapScale) { setBumpMapScaleU(bumpMapScale); setBumpMapScaleV(bumpMapScale); }
	void setBumpMapScaleU(float bumpMapScaleU) { if (fabs(bumpMapScaleU) > 1e-3) bumpMapScaleU_ = bumpMapScaleU; }
	void setBumpMapScaleV(float bumpMapScaleV) { if (fabs(bumpMapScaleV) > 1e-3) bumpMapScaleV_ = bumpMapScaleV; }

	void setRefractionIndex(float refractionIndex){ refractionIndex_ = refractionIndex; }

	void initFromDOMElement(const QDomElement& e);
  
private:
	Color diffuseColor_, specularColor_, reflectiveColor_;
	float specularCoefficient_, textureScaleU_, textureScaleV_,bumpMapScaleU_, bumpMapScaleV_;
	TextureMode textureMode_;
	QImage texture_;
	QImage bump_map_;
	float refractionIndex_; /*!< Indice optique du milieu composé par le matériau (1 dans le vide) */
};

#endif // MATERIAL_H
