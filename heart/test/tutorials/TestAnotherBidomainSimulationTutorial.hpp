/*

Copyright (c) 2005-2012, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/*
 *
 *  Chaste tutorial - this page gets automatically changed to a wiki page
 *  DO NOT remove the comments below, and if the code has to be changed in
 *  order to run, please check the comments are still accurate
 *
 *
 */
#ifndef TESTANOTHERBIDOMAINSIMULATIONTUTORIAL_HPP_
#define TESTANOTHERBIDOMAINSIMULATIONTUTORIAL_HPP_
/*
 * = Another example showing how to run a bidomain simulation =
 *
 * In this tutorial we run another bidomain simulation,
 * showing (i) an example using one of the source cell factories, (ii) how to define
 * and use fibre directions, and (iii) mentioning how to write other output file formats.
 *
 * EMPTYLINE
 *
 * The first thing to do is to include the headers as before.
 */
#include <cxxtest/TestSuite.h>
#include "BidomainProblem.hpp"
#include "LuoRudy1991.hpp"
#include "PetscSetupAndFinalize.hpp"
/* Rather than write our own cell factory this time, we will use
 * the {{{PlaneStimulusCellFactory}}}. */
#include "PlaneStimulusCellFactory.hpp"

/* Now we define the test class, which must inherit from {{{CxxTest::TestSuite}}}
 * as usual, and the (public) test method
 */
class TestAnotherBidomainTutorial : public CxxTest::TestSuite
{
public:
    void TestFibreSimulation() throw(Exception)
    {
        /* It is not the case here, but if there were other tests in the file that
         * had already been run and might have changed parameters in `HeartConfig`, we
         * would need to call `Reset` */
        HeartConfig::Instance()->Reset();

        /* Next, we have to create a cell factory of the type we defined above. The plane
         * stimulus cell factory sets up cells of the given type with a non-zero stimulus
         * for cells on the x=0 boundary. The 2 below is the dimension, and the -2000000
         * is the stimulus magnitude (the default stimulus duration is used).
         */
        PlaneStimulusCellFactory<CellLuoRudy1991FromCellML,2> cell_factory(-2000000);

        /* Define an end time, output directory and prefix as before */
        HeartConfig::Instance()->SetSimulationDuration(5.0); //ms
        HeartConfig::Instance()->SetOutputDirectory("BidomainFibresTutorial");
        HeartConfig::Instance()->SetOutputFilenamePrefix("results");

        /* HOW_TO_TAG Cardiac/Problem definition
         * Specify fibre directions
         */
        /* Define a mesh to be read, saying that we also want to read fibres. The extra part can either be
         * `cp::media_type::Orthotropic`, in which case `2D_0_to_1mm_800_elements.ortho` will also be read;
         * or `cp::media_type::Axisymmetric`, in which case `2D_0_to_1mm_800_elements.axi` will also be read.
         * See the file formats documentation for full descriptions of these formats, but basically .axi
         * files provide the fibre direction for each element in the mesh, and .ortho files provide the fibre,
         * sheet (and normal in 3D) directions for each element in the mesh. */
        HeartConfig::Instance()->SetMeshFileName("mesh/test/data/2D_0_to_1mm_800_elements", cp::media_type::Orthotropic);

        /* The fibre file provided here defines (non-physiological) 'kinked' fibres which are
         * in the x-direction for x<0.05, and then the diagonal (1,1) direction for x>0.05.
         * It was generated by looping over the mesh that is to be used, as shown here.
         * The output is the .ortho file minus the header.
         */
//        TetrahedralMesh<2,2> mesh;
//        TrianglesMeshReader<2,2> reader("mesh/test/data/2D_0_to_1mm_800_elements");
//        mesh.ConstructFromMeshReader(reader);
//        for(unsigned i=0; i<mesh.GetNumElements(); i++)
//        {
//            double x = mesh.GetElement(i)->CalculateCentroid()[0];
//            if(x<0.05)
//            {
//                std::cout << "1 0 0 1\n";
//            }
//            else
//            {
//                std::cout << "0.707106781 0.707106781 -0.707106781 0.707106781\n";
//            }
//        }
//        return;

        /* This is of course not enough - by default isotropic conductivities are used so the
         * variable fibre directions won't make any difference - so we have to alter the
         * intracellular and extracellular conductivities to be anisotropic. The first value in
         * the following is the conductivity in the fibre direction, the second the conductivity
         * in the sheet direction (and the third in 3d would be that in the normal direction).
         * Note, we have scaled all the conductivities from the physiological values as the mesh
         * is a little too coarse to be able to handle the smallest conductivities (try running with
         * scale = 1 to see the error message).
         */
        double scale = 2;
        HeartConfig::Instance()->SetIntracellularConductivities(Create_c_vector(1.75*scale, 0.19*scale));
        HeartConfig::Instance()->SetExtracellularConductivities(Create_c_vector(7.0*scale, 2.4*scale));

        /* HOW_TO_TAG Cardiac/Problem definition
         * Specify output formats (for different visualisers)
         */

        /* The output will be written to /tmp/USER_NAME/testoutput/BidomainTutorial
         * in hdf5 format, and converted to meshalyzer format at the end of the simulation.
         * To adjust this, or convert to Cmgui or VTK format as well, use methods in
         * `HeartConfig`,  e.g.
         */
        //HeartConfig::Instance()->SetVisualizeWithMeshalyzer(false);
        HeartConfig::Instance()->SetVisualizeWithCmgui(true);
        /* The other option is to write in VTK format (which needs VTK installed), following
         * which the results can be loaded in the visualiser Paraview */
        //HeartConfig::Instance()->SetVisualizeWithVtk(true);
        /* If the mesh is a DistributedTetrahedralMesh then we can use parallel VTK files (.pvtu)*/
        //HeartConfig::Instance()->SetVisualizeWithParallelVtk(true);

        /* Now we create a problem class, initialise and solve */
        BidomainProblem<2> bidomain_problem( &cell_factory );

        bidomain_problem.Initialise();
        bidomain_problem.Solve();

        /* The results can now be visualised - the effect of the fibres changing direction at x=0.05
         * on the wave should be very clear.
         *
         * EMPTYLINE
         *
         * We described in the previous tutorial how to access the latest voltage vector using
         * `ReplicatableVector`, here we illustrate how to access the voltage values using the
         * {{{DistributedVector}}} class, which can be used to only iterate over the values
         * of the voltage owned by that process (for parallel simulations).
         */
        DistributedVector dist_bidomain_voltage = bidomain_problem.GetSolutionDistributedVector();
        DistributedVector::Stripe bidomain_voltage(dist_bidomain_voltage, 0);
        DistributedVector::Stripe extracellular_potential(dist_bidomain_voltage, 1);

        /* A loop over all the components owned by this process.. */
        for (DistributedVector::Iterator index = dist_bidomain_voltage.Begin();
             index != dist_bidomain_voltage.End();
             ++index)
        {
            /* .. and a simple test, that the 'last' node was stimulated: */
            if (index.Global==bidomain_problem.rGetMesh().GetNumNodes()-1) // ie if the last node
            {
                TS_ASSERT_LESS_THAN(0, bidomain_voltage[index]);
            }
        }
    }
};

#endif /*TESTANOTHERBIDOMAINSIMULATIONTUTORIAL_HPP_*/
