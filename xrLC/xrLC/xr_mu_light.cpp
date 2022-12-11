#include "stdafx.h"
#include "build.h"

void MuLight()
{
	CTimer	start_mu1;
	start_mu1.Start();

	// Light models
	
	int Mu_Size = pBuild->mu_models.size();

	for (int m = 0; m < Mu_Size; m++)
	{
		//Progress(float(m) / float(Mu_Size));
		pBuild->mu_models[m]->calc_materials();
		pBuild->mu_models[m]->calc_lighting();
	}

	clMsg("mu_light primary completed %f seconds elapsed", start_mu1.GetElapsed_sec());	


		// Light references
		int count = (int)pBuild->mu_refs.size();

		if (count)
		{
			CTimer	start_mu;
			start_mu.Start();

			for (int it = 0; it < count; it++)
			{
				pBuild->mu_refs[it]->calc_lighting();
			}

			clMsg("mu_light secondary completed %f seconds elapsed", start_mu.GetElapsed_sec());
		}
	}

void	CBuild::xrPhase_MU_light()
{
	MuLight();
}