#pragma once

#include "DatabaseTable.h"

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// Interface for objects that store information in a database
		/// </summary>
		class IDatabaseObject
		{
		protected:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="database">Database implementation</param>
			IDatabaseObject() {};

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~IDatabaseObject() = default;
		public:
			/// <summary>
			/// Load from available data
			/// </summary>
			virtual int load(std::vector<std::string>& rawData) = 0;

			/// <summary>
			/// Create dependent data for new items
			/// </summary>
			/// <returns></returns>
			virtual int create_dependent_data() { return 0; }

			/// <summary>
			/// Should remove all data that depends on this item. After removing the item
			/// the dependence is lost and therefore useless.
			/// </summary>
			/// <returns></returns>
			virtual int remove_dependent_data() { return 0; }

			/// <summary>
			/// Returns the DatabaseTable corresponding to the class that is implementing
			/// </summary>
			/// <returns></returns>
			virtual DatabaseTable& get_table_structure() = 0;

		protected:
			/// <summary>
			/// Returns true if model can be saved
			/// </summary>
			virtual bool canSave() { return true; }
		};
	}
}
