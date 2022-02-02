%define LAST_ELEMENT 0

%macro colon 2
    %ifid %2
        %%next_label: dq LAST_ELEMENT
        %define LAST_ELEMENT %%next_label
    %else
        %fatal "Error: Bad label format. It should be ID."
    %endif

    %ifstr %1
        db %1, 0
        %2:
    %else
        %fatal "Error: Bad key format. It should be string."
    %endif
%endmacro