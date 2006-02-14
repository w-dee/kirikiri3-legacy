//---------------------------------------------------------------------------
/*
	Risa [�肳]      alias �g���g��3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risa�̃A�C�R���Ȃǂ̃v���o�C�_
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/utils/Singleton.h"
#include <wx/artprov.h>

RISSE_DEFINE_SOURCE_ID(1864,60513,53990,19952,46254,61777,48308,42907);



//---------------------------------------------------------------------------
// ���̃v���o�C�_���񋟂���r�b�g�}�b�v
//---------------------------------------------------------------------------
#include "right_triangle.xpm"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief Risa�pArtProvider
//---------------------------------------------------------------------------
class tRisaArtProvider : public wxArtProvider
{
protected:
	virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
									const wxSize& size);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief �r�b�g�}�b�v���쐬���ĕԂ�
//---------------------------------------------------------------------------
wxBitmap tRisaArtProvider::CreateBitmap(const wxArtID& id,
									 const wxArtClient& client,
									 const wxSize& WXUNUSED(size))
{
	if(id == wxT("RisaRightTriangle"))
		return wxBitmap(right_triangle_xpm);
	return wxNullBitmap;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tRisaArtProvider ��o�^���邽�߂̃V���O���g��
//---------------------------------------------------------------------------
class tRisaArtProviderRegisterer
{
	tRisaArtProvider * Provider;
public:
	//! @brief �R���X�g���N�^
	tRisaArtProviderRegisterer()
	{
		Provider = new tRisaArtProvider;
		wxArtProvider::PushProvider(Provider);
	}

private:
	tRisaSingletonObjectLifeTracer<tRisaArtProviderRegisterer> singleton_object_life_tracer;

public:
	static boost::shared_ptr<tRisaArtProviderRegisterer> & instance() { return
		tRisaSingleton<tRisaArtProviderRegisterer>::instance();
			} //!< ���̃V���O���g���̃C���X�^���X��Ԃ�
};
//---------------------------------------------------------------------------


