/*
  Copyright 2016 SINTEF ICT, Applied Mathematics.
  Copyright 2016 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPM_ECLWELLSOLUTION_HEADER_INCLUDED
#define OPM_ECLWELLSOLUTION_HEADER_INCLUDED

#include <ert/ecl/ecl_file.h>
#include <ert/util/ert_unique_ptr.hpp>
#include <boost/filesystem.hpp>
#include <array>
#include <string>
#include <utility>
#include <vector>

namespace Opm
{

    class ECLWellSolution
    {
    public:
        /// Construct with path to restart file.
        /// \param[in] restart_filename    filename of UNRST file
        /// \param[in] rate_threshold      a well will be ignored if its total RESV rate is less than this
        /// \param[in] disallow_crossflow  if true, injecting perforations of production wells and vice versa will be ignored
        explicit ECLWellSolution(const boost::filesystem::path& restart_filename,
                                 const double rate_threshold = 1e-14,
                                 const bool disallow_crossflow = true);

        /// Contains the well data extracted from the restart file.
        struct WellData
        {
            std::string name;
            bool is_injector_well;
            struct Completion
            {
                int grid_index;                // 0 for main grid, otherwise LGR grid.
                std::array<int, 3> ijk;        // Cartesian location in grid.
                double reservoir_inflow_rate;  // Total fluid rate in SI (m^3/s).
            };
            std::vector<Completion> completions;
        };

        /// Return well solution for given report step.
        ///
        /// Will throw if required data is not available for the
        /// requested step.
        std::vector<WellData> solution(const int report_step,
                                       const int num_grids) const;

    private:
        // Types.
        using FilePtr = ERT::ert_unique_ptr<ecl_file_type, ecl_file_close>;

        // Data members.
        FilePtr restart_;
        double rate_threshold_;
        bool disallow_crossflow_;

        // Methods.
        ecl_kw_type* getKeyword(const std::string& fieldname) const;
        std::vector<double> loadDoubleField(const std::string& fieldname) const;
        std::vector<int> loadIntField(const std::string& fieldname) const;
        std::vector<std::string> loadStringField(const std::string& fieldname) const;
        std::vector<WellData> readWellData(const int grid_index) const;
    };


} // namespace Opm

#endif // OPM_ECLWELLSOLUTION_HEADER_INCLUDED