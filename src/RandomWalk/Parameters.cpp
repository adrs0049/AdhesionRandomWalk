
#include "Parameters.h"
#include "debug.h"

Parameters::Parameters() {}

Parameters::Parameters(double Dsize_, unsigned int DomainN_,
        double FinalTime_, unsigned long _ic_p)
: DomainSize(Dsize_), DomainSizeL(0),
    DomainN(DomainN_), FinalTimes(1, FinalTime_),
    ic_p(_ic_p)
{
    // TODO check if this is correct
    domainShape.resize(2, 0);
    domainShape[0] = -DomainSize/2;
    domainShape[1] = DomainSize/2;

    NumberOfCells = 0;

    update();
    print_info();
}

Parameters::Parameters(double Dsize_, unsigned int DomainN_,
        std::vector<double> FinalTimes_, unsigned long _ic_p)
: DomainSize(Dsize_), DomainSizeL(0),
    DomainN(DomainN_), FinalTimes(FinalTimes_),
    ic_p(_ic_p)
{
    // TODO check if this is correct
    domainShape.resize(2, 0);
    domainShape[0] = -DomainSize/2;
    domainShape[1] = DomainSize/2;

    NumberOfCells = 0;

    update();
    print_info();
}

Parameters::Parameters(std::vector<double> _shape, unsigned int _DomainN,
        double _finalTime,
        unsigned long _ic_p)
: domainShape(_shape), DomainN(_DomainN), FinalTimes(1, _finalTime),
    ic_p(_ic_p)
{
    ASSERT(_shape.size()==2, "shape size of " << _shape.size() << " is invalid!");

    DomainSize = domainShape[1] - domainShape[0];

    update();
    print_info();
}

Parameters::Parameters(std::vector<double> _shape, unsigned int _DomainN,
        std::vector<double> FinalTimes_,
        unsigned long _ic_p)
: domainShape(_shape), DomainN(_DomainN),
    FinalTimes(FinalTimes_), ic_p(_ic_p)
{
    ASSERT(_shape.size()==2, "shape size of " << _shape.size() << " is invalid!");

    DomainSize = domainShape[1] - domainShape[0];

	std::cerr << "update" << std::endl;
    update();
    print_info();
}

void Parameters::print_info()
{
    Check();
    std::cout << "Domain Size:" << DomainSize;
    std::cout << " Domain Size L:" << DomainSizeL;
    std::cout << " Domain = (" << domainShape[0] << ", "
        << domainShape[1] << ").";
    std::cout << " StepSize:" << StepSize << std::endl;
    std::cout << " Sensing Radius:" << SensingRadiusL;
    std::cout << " FinalTimes=(";
    std::copy(FinalTimes.begin(), FinalTimes.end(),
            std::ostream_iterator<double>(std::cout, ", "));
    std::cout << ")" << std::endl;
}

void Parameters::update()
{
    // sort elements of FinalTimes, required!!
    std::sort(FinalTimes.begin(), FinalTimes.end());

	if (base2)
	{
		std::cerr << "Base2 is on!";
		std::cerr << " domainSize=" << DomainN;

		DomainN--;
		DomainN |= DomainN >> 1;
		DomainN |= DomainN >> 2;
		DomainN |= DomainN >> 4;
		DomainN |= DomainN >> 8;
		DomainN |= DomainN >> 16;
		DomainN++;

		std::cerr << " domainSize=" << DomainN;

		StepSize = 1.0 / DomainN;
		DomainSizeL = DomainSize / StepSize;
		SensingRadiusL = SensingRadius / StepSize;

		std::cerr << " SesningRad=" << SensingRadiusL << std::endl;
	}
	else
	{
		StepSize = 1.0 / DomainN;
		DomainSizeL = DomainSize / StepSize;
		SensingRadiusL = SensingRadius / StepSize;
	}
    lambda = 1E2;
    set = true;

	// FIXME
	if (DomainN % 2 == 0)
		base2 = true;

	Check();
}

void Parameters::Check()
{
    ASSERT(FinalTimes.size()>0, "FinalTimes are not set");
    ASSERT(set, "Parameters are not initalized.");
    ASSERT(domainShape.size()==2, "Domain shape has invalid size!");
	ASSERT(DomainSize>0.0, "DomainSize has to be larger than zero!");
	ASSERT(DomainSizeL>0, "DomainSizeL has to be larger than zero!");
    ASSERT(StepSize>0.0, "StepSize has to be larger than zero!");

	ASSERT(ic_p!=0, "Initial number of cells can't be zero!");
    ASSERT(0.0<=InitCellDensity && InitCellDensity<=1.0, "Initial Cell Density " << InitCellDensity << " is invalid. The valid range is [0,1].");
    ASSERT(Diffusion>0.0, "The diffusion coefficient can't be negative");

	if (rw_type == RANDOMWALK_TYPE::DIFFUSION_AND_DRIFT ||
		rw_type == RANDOMWALK_TYPE::ADHESION)
	{
		ASSERT(Drift>0.0, "The drift coefficient has to be positive!");
	}

	if (rw_type == RANDOMWALK_TYPE::ADHESION)
	{
		ASSERT(base2, "For simulation type adhesion base2 has to be set!");
		ASSERT(omega_p>0.0, "Omega p must be larger than 0");

		ASSERT(SensingRadiusL>0, "SensingRadiusL has to be positive");
		ASSERT(SensingRadius>0.0, "SensingRadius has to be positive!");

		ASSERT(SensingRadius < DomainSize/2, "SensingRadius is too large!");
		ASSERT(SensingRadiusL < DomainSizeL/2, "SensingRadiusL is too large!");
	}
}
