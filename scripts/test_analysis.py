from pandas import DataFrame
from pandas import Series

from analysis import explode_csv_series
from analysis import filter_dataframe
from analysis import log_as_dataframe
from analysis import LOG_COLUMNS


def test_log_as_dataframe_empty():
    """
    Even if the log is empty, a DataFrame should be created with the correct
    columns and index.
    """
    df = log_as_dataframe([])
    assert isinstance(df, DataFrame)
    index, *columns = LOG_COLUMNS
    assert df.index.name == index
    assert list(df.columns) == columns


def test_explode_csv_series():
    """
    Test `explode_csv_series()` function.
    """
    series = Series(['1,0,-1', '2,0,-2'])
    df = explode_csv_series(series)
    assert df.shape == (2, 3)
    assert list(df.iloc[0]) == [1, 0, -1]
    assert list(df.iloc[1]) == [2, 0, -2]


def test_explode_csv_series_empty():
    """
    Test `explode_csv_series()` function when passing an empty Series.
    """
    series = Series()
    df = explode_csv_series(series)
    assert isinstance(df, DataFrame)


def test_filter_dataframe():
    """
    Test `filter_dataframe()` function.
    """
    df = DataFrame(DataFrame({'a': [1, 2, 1], 'b': [2, 3, 2], 'c': [0, 1, 2]}))
    result = filter_dataframe(df, {'a': 1, 'b': 2})
    # Double match
    assert len(result) == 2
    assert all(result['a'] == 1)
    assert all(result['b'] == 2)
    # No results
    result = filter_dataframe(df, {'a': 1, 'b': 2, 'c': 1})
    assert len(result) == 0
