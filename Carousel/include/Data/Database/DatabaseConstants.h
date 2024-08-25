#pragma	once

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// Default value for Ids
		/// </summary>
		static const int DEFAULT_ID{ -1 };

		/// <summary>
		/// Primary key index.
		/// Index where the primary key value is found.
		/// </summary>
		static const int PK_INDEX{ 0 };

		/// <summary>
		/// Database contraint types
		/// </summary>
		enum DatabaseConstraintType
		{
			COLUMN		= 1 << 0,
			PRIMARY_KEY = 1 << 1,
			UNIQUE		= 1 << 2,
			LAST		= 2,
			FIRST		= 0,
		};

		/// <summary>
		/// Bitwise operations for DatabaseConstraintType
		/// </summary>
		/// <param name="left"></param>
		/// <param name="right"></param>
		/// <returns></returns>
		inline DatabaseConstraintType operator|(DatabaseConstraintType left, DatabaseConstraintType right)
		{
			return static_cast<DatabaseConstraintType>(static_cast<int>(left) | static_cast<int>(right));
		}
	}
}