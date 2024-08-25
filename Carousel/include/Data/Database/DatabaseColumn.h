#pragma once

#include <string>
#include <functional>
#include <cassert>
#include <algorithm>
#include "DatabaseConstants.h"
#include "../../Helpers/Converters.h"

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// Represents a column in a table.
		/// </summary>
		class DatabaseColumn
		{
		private:
			/// <summary>
			/// Column name
			/// </summary>
			std::string _name;

			/// <summary>
			/// Column data type (from typeid)
			/// </summary>
			std::string _type;

			/// <summary>
			/// Column constraint type
			/// </summary>
			DatabaseConstraintType _constraint{ DatabaseConstraintType::COLUMN };

			/// <summary>
			/// Getter method for this column from object
			/// </summary>
			std::function<std::string(void*)> _getterFunction;

			/// <summary>
			/// Sets value for this column onto object
			/// </summary>
			std::function<void(void*, const std::string&)> _setterFunction;

		public:
			/// <summary>
			/// Constructor
			/// </summary>
			DatabaseColumn(std::string name, std::string type, DatabaseConstraintType constraint)
				: _name(std::move(name)), _type(std::move(type)), _constraint(constraint)
			{
				// Empty
			}

		public:
			/// <summary>
			/// Returns the column name
			/// </summary>
			const std::string& getName() { return _name; }

			/// <summary>
			/// Returns the column type
			/// </summary>
			/// <returns></returns>
			const std::string& getType() { return _type; }

			/// <summary>
			/// Returns the type of constraint for said column
			/// </summary>
			const DatabaseConstraintType& getConstraint() const { return _constraint; }

			/// <summary>
			/// Sets getter method for getting column data based on object
			/// </summary>
			template<typename T, typename R>
			void setGetter(const R& (T::* getter)())
			{
				_getterFunction = [getter](void* obj) -> std::string {
					T* typedObj = static_cast<T*>(obj);
					return carousel::helpers::converters::convertToString((typedObj->*getter)());
					};
			}

			/// <summary>
			/// Sets setter method for setting column data based on object
			/// </summary>
			template<typename T, typename R>
			void setSetter(void (T::* setter)(const R&)) {
				_setterFunction = [setter](void* obj, const std::string& value) {
					T* typedObj = static_cast<T*>(obj);
					(typedObj->*setter)(carousel::helpers::converters::convertFromString<R>(value));
					};
			}

			/// <summary>
			/// Gets value from object
			/// </summary>
			std::string getValue(void* obj) const {
				assert(_getterFunction);
				return _getterFunction(obj);
			}

			/// <summary>
			/// Sets value to object
			/// </summary>
			void setValue(void* obj, const std::string& value) const {
				assert(_setterFunction);
				_setterFunction(obj, value);
			}
		};
	}
}