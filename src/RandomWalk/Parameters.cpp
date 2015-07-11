
#include "Parameters.h"
#include "debug.h"

Parameters::Parameters() {}

Parameters::Parameters(double Dsize_, double StepSize_,
        double FinalTime_, unsigned long _ic_p)
: DomainSize(Dsize_), DomainSizeL(0),
    StepSize(StepSize_), FinalTimes(1, FinalTime_),
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

Parameters::Parameters(double Dsize_, double StepSize_,
        std::vector<double> FinalTimes_, unsigned long _ic_p)
: DomainSize(Dsize_), DomainSizeL(0),
    StepSize(StepSize_), FinalTimes(FinalTimes_),
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

Parameters::Parameters(std::vector<double> _shape, double _stepSize,
        double _finalTime,
        unsigned long _ic_p)
: domainShape(_shape), StepSize(_stepSize), FinalTimes(1, _finalTime),
    ic_p(_ic_p)
{
    ASSERT(_shape.size()==2, "shape size of " << _shape.size() << " is invalid!");

    DomainSize = domainShape[1] - domainShape[0];

    update();
    print_info();
}

Parameters::Parameters(std::vector<double> _shape, double _stepSize,
        std::vector<double> FinalTimes_,
        unsigned long _ic_p)
: domainShape(_shape), StepSize(_stepSize),
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
    DomainSizeL = DomainSize / StepSize;
    SensingRadiusL = SensingRadius / StepSize;
    lambda = 1E2;
    set = true;

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
	ASSERT(SensingRadius < DomainSize/2, "SensingRadius is too large!");
	ASSERT(SensingRadiusL < DomainSizeL/2, "SensingRadiusL is too large!");

	WARNING(SensingRadiusL==0, "SensingRadiusL is zero!");
	WARNING(SensingRadius==0.0, "SensingRadius is zero!");

    ASSERT(ic_p!=0, "Initial number of cells can't be zero!");
    ASSERT(0.0<=InitCellDensity && InitCellDensity<=1.0, "Initial Cell Density " << InitCellDensity << " is invalid. The valid range is [0,1].");
    ASSERT(Diffusion>=0.0, "The diffusion coefficient can't be negative");
	WARNING(Diffusion==0.0, "The diffusion coefficient is zero!");
}
