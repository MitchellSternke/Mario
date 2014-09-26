#ifndef SINGLETON_HPP
#define SINGLETON_HPP

/**
* A global instance template for classes.
*
* @tparam T the class type to store in the Singleton.
*/
template <class T>
class Singleton
{
public:
	/**
	 * Create a new instance of the Singleton.
	 */
	static void createInstance()
	{
		instance = new T();
	}

	/**
	 * Destroy the current instance of the Singleton.
	 */
	static void destroyInstance()
	{
		delete instance;
		instance = 0;
	}

	/**
	 * Get the current instance of the Singleton.
	 *
	 * @return a reference to the current instance.
	 */
	static T& getInstance()
	{
		return *instance;
	}

	/**
	 * Set the instance used by the Singleton.
	 *
	 * @param newInstance the new instance to use.
	 */
	static void setInstance(T* newInstance)
	{
		if( instance )
			destroyInstance();
		instance = newInstance;
	}

private:
	static T* instance;

	Singleton() {}
	~Singleton() {}
};

template <class T>
T* Singleton<T>::instance = 0;

#endif // SINGLETON_HPP
